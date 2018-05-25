#include "myClientClass.h"
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>

#include <iostream>

#define mqtt_host "192.168.0.65"
#define mqtt_port 1883

using namespace std;

// MQTT Stuff
//
void connect_callback(struct mosquitto *mosq, void *obj, int result) {
        printf("connect callback, rc=%d\n", result);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
        printf("message callback\n");
        char sql[BUFFER_SIZE];

        myClient *me = (myClient *)obj;

        printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

        sprintf(sql,"update io_point set state='%s'  where topic='%s';\n",(char*) message->payload, message->topic);

        printf("%s\n", sql);

        me->sendCmd( sql );
}

void usage() {
    printf("Usage\n");
}

int main(int argc, char *argv[]) {
    bool verbose=false;

    int len=0;
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];

    string hostName = "localhost";
    string serviceName ="myclient" ;

    int opt;
    int rc;

    while (( opt = getopt(argc, argv, "h?n:p:v")) !=-1) {
        switch(opt) {
            case '?':
            case 'h':
                usage();
                exit(0);
            case 'n':
                hostName = optarg;
                break;
            case 'p':
                serviceName = optarg;
                break;
            case 'v':
                verbose = true;
                break;
        }
    }

    if(verbose) {
        cout << "Hostname : " << hostName << endl;
        cout << "Service  : " << serviceName << endl;
    }

    myClient *n = myClient::Instance();

    bool failFlag = n->setupNetwork((char *)hostName.c_str(), (char *)serviceName.c_str());

    if(verbose) {
        printf("Network setup %s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }

    if(failFlag) {
        exit(1);
    }

    failFlag = n->loadFile( "mysqlCmds.txt");

    if(verbose) {
        cout << "loadFile ";
        printf("%s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }

    failFlag = n->clientConnect() ;

    if(verbose) {
        cout << "clientConnect ";
        printf("%s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }

    if ( n->clientConnected() == false ) {
        fprintf(stderr, "FATAL ERROR: Connected to interface but not to database\n");
        exit(2);
    }
    // 
    // OK, all connected to database and ready to go.
    // Time now to setup MQTT
    //
    mosquitto_lib_init();

    char *clientId = basename(argv[0]);
    struct mosquitto *mosq = mosquitto_new( clientId, true, (void *)n);

    if( !mosq ) {
        fprintf(stderr,"FATAL ERROR: Failed to setup mosquitto connection\n");
        exit(3);
    }

    mosquitto_connect_callback_set(mosq, connect_callback);
    mosquitto_message_callback_set(mosq, message_callback);

    rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

    string out;
    string sql = "select topic from io_point where direction = 'in';\n";
    len = n->sendCmd( sql );

    string cmd = "^get-row-count\n";
    len = n->sendCmd( cmd, out );

    int cnt = stoi( out,nullptr);

    cout << cnt << endl;

    for( int i=0; i < cnt ; i++ ) {
        if( i == 0 ) {
            cmd = "^get-col topic\n";
        } else {
            cmd = "^go-next\n";
            len = n->sendCmd( cmd );
            cmd = "^get-col topic\n";
        }
        len = n->sendCmd( cmd, out );

        cout << out << endl;
        mosquitto_subscribe(mosq, NULL,(char *)out.c_str() , 0);
    }

    rc = mosquitto_loop_forever(mosq, -1, 1);


    bool run=true;
    while(run) {
        rc = mosquitto_loop(mosq, -1, 1);
        if(run && (rc == 0)) {
            printf("connection error!\n");
            printf("%s\n", mosquitto_strerror( rc ));
            sleep(10);
            mosquitto_reconnect(mosq);
        }
    }
    mosquitto_destroy(mosq);
}

