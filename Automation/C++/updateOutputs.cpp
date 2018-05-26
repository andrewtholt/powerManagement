#include "myClientClass.h"
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>
#include <sys/types.h>
#include <signal.h>


#include <iostream>

#define mqtt_host "192.168.0.65"
#define mqtt_port 1883

using namespace std;
int logicPid=0;

void alarmHandler(int sig) {
    signal(SIGUSR1, SIG_IGN);
    printf("Alarm fired\n");
    signal(SIGUSR1, alarmHandler);
}

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
    string topic;
    string state;
    bool firstTime=true;

    pid_t iam=getpid();
    string myName = basename(argv[0]);

    string pidFile = "/var/tmp/" + myName;

    FILE *fd = fopen( pidFile.c_str(), "w");
    if( fd == NULL ) {
        fprintf(stderr, "FATAL ERROR: Failed to open pidFile\n");
        exit(3);
    }

    fprintf(fd, "%d\n" , iam);
    fclose(fd);

    signal(SIGUSR1, alarmHandler);

    while(true) {

        if ( firstTime ) {
            firstTime=false;
        } else {
            pause();
        }
        string sql = "select topic, state from io_point where direction = 'out';\n";
        len = n->sendCmd( sql );

        string cmd = "^get-row-count\n";
        len = n->sendCmd( cmd, out );

        int cnt = stoi( out,nullptr);

        cout << cnt << endl;

        for( int i=0; i < cnt ; i++ ) {
            if( i > 0 ) {
                cmd = "^go-next\n";
                len = n->sendCmd( cmd );
            }
            cmd = "^get-col topic\n";
            len = n->sendCmd( cmd, topic );
            cmd = "^get-col state\n";
            len = n->sendCmd( cmd, state );

            cout << "Topic : " + topic << endl;
            cout << "Msg   : " + state << endl;

            mosquitto_publish(mosq, NULL, topic.c_str(), state.length(), state.c_str(),0,true);

        }
    }
}

