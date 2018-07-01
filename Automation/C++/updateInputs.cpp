#include "myClientClass.h"
#include "semaphore.h"

#include <string.h>
#include <unistd.h>
#include <mosquitto.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#include <hspread.h>

#include <iostream>

#define mqtt_host "192.168.0.65"
#define mqtt_port 1883

using namespace std;
int logicPid=0;
int sid;
string destGroup = "logic";
int tickDelay=30;

int calcDelay(int q) {
    time_t now=time(NULL);
    struct tm *hms = localtime( &now );
    int seconds = hms->tm_sec ;
    int delay = q - ( seconds % q)+1;

    printf ("%02d:%02d:%02d\n", hms->tm_hour, hms->tm_min, hms->tm_sec);
    printf("Delay=%d\n", delay);

    return delay;
}
//
// alarm handler
//
void alarmHandler(int sig) {
    static int count=0;
    signal(SIGALRM, SIG_IGN);
    count++;
    printf("updateInputs: Alarm fired %d\n", count);
    int rc =  SPTxSimple((char *)destGroup.c_str(), (char *)"TICK");
    signal(SIGALRM, alarmHandler);
    alarm(calcDelay(tickDelay));
}

// MQTT Stuff
//
void connect_callback(struct mosquitto *mosq, void *obj, int result) {
    printf("connect callback, rc=%d\n", result);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    time_t old = 0;
    time_t now = 0;
    char sql[BUFFER_SIZE];
    string spreadMsg;
    string out;
    string onValue;
    string offValue;

    myClient *me = (myClient *)obj;

    printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

    sprintf(sql, "select name,on_state,off_state from io_point where topic = '%s';\n", message->topic);
    old=time(NULL);
    me->sendCmd( sql );
    now=time(NULL);

    me->sendCmd( (char *)"^get-col name\n", out);
    spreadMsg = out + " ";

    me->sendCmd( (char *)"^get-col on_state\n",  onValue);
    me->sendCmd( (char *)"^get-col off_state\n", offValue);

    if( !strcmp((char *)message->payload,(char *)onValue.c_str())) {
        spreadMsg += "TRUE";
    } else if( !strcmp((char *)message->payload,(char *)offValue.c_str())) {
        spreadMsg += "FALSE";
    } else {
        spreadMsg += "UNKNOWN";
    }


    cout << destGroup + " " + spreadMsg << endl;
    int rc =  SPTxSimple((char *)destGroup.c_str(), (char *)spreadMsg.c_str()) ;

    printf("Sending to %s\n",(char *)destGroup.c_str());
    printf("           %s\n",(char *)spreadMsg.c_str());

    if ( logicPid > 0) {
        kill( logicPid, SIGALRM);
    }
    usleep(1000 * 1000);
}

void usage() {
    printf("Usage\n");
    printf("\t-g <spread group>\tSpread group to send message to\n");
    printf("\t-h|?\t\tHelp\n");
    printf("\t-i <name>\tMy name for spread\n");
    printf("\t-n <name>\tDatabase hostname\n");
    printf("\t-p <svc name>\tDatabase service name\n");
    printf("\t-s <spread host>\tHostname of spread host\n");
    printf("\t-t <delay>\tTime between ticks, aligned to 0 seconds.\n");
    printf("\t-v\t\tVerbose\n");
    printf("\t-w\t\tWait for logic.\n");
}

int main(int argc, char *argv[]) {
    bool verbose=false;
    bool waitForLogic=false;

    int len=0;
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];

    string hostName = "localhost";
    string serviceName ="myclient" ;

    string spreadHost = "localhost" ;
    string myName = "updateInputsMQTT";
    int spreadPort=4803;

    int opt;
    int rc;

    while (( opt = getopt(argc, argv, "g:h?i:n:p:s:t:vw")) !=-1) {
        switch(opt) {
            case 'g':
                // Spread group to send messages to.
                destGroup = optarg;
                break;
            case '?':
            case 'h':
                usage();
                exit(0);
            case 'i':
                myName = optarg;
                break;
            case 'n':
                hostName = optarg;
                break;
            case 'p':
                serviceName = optarg;
                break;
            case 's':
                spreadHost = optarg;
                break;
            case 't':
                tickDelay = atoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case 'w':
                waitForLogic = true;
                break;
        }
    }


//    if(verbose) {
        cout << "Hostname    : " << hostName << endl;
        cout << "Service     : " << serviceName << endl;
        cout << "Spread Host : " << spreadHost << endl;
        cout << "I am        : " << myName << endl;
        cout << "Tick Delay  : " << tickDelay << endl;
        cout << endl;
//    }

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

    while ( n->clientConnected() == false ) {
        if( verbose ) {
            fprintf(stderr, "ERROR: Connected to interface but not to database\n");
        }
        sleep(2);
    }
    // Now connect to spread.
    //
    //
    setUser((char *)myName.c_str());
    setServer((char *)spreadHost.c_str());
    dump();

    rc=SPConnectSimple();
    printf("SPConnectSimple rc=%d\n", rc);

    if( rc < 0 ) {
        printf("Spread connect failed\n");
        dump();
        exit(1);
    }

    rc=SPJoinSimple((char *)"global");


    sleep(1);

    // 
    // OK, all connected to database and and spread, so ready to go.
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

    int key = 42;
    sid = semtran( key );

    // If 0 then locked, created locked.
    printf("Semaphore value is %d\n", getSemValue(sid));

    if( sid < 0 ) {
        perror("FATAL ERROR: semtran");
        exit(4);
    }

    bool gotLogicPid=false;

    if( waitForLogic) {
        do {
            FILE *fd = fopen("/var/tmp/logic", "r");

            if( fd == NULL ) {
                printf("Can't open logic pid file, sleeping .... zzz\n");
                sleep(5);
            } else {
                char pidBuffer[16];
                char *rc = fgets( pidBuffer, 16, fd);
                fclose( fd );
                logicPid = atoi( pidBuffer );

                int ok = kill(logicPid, 0);

                gotLogicPid = ( ok == 0) ? true : false ;

                if( gotLogicPid == false ) {
                    printf("Can't contact logic, sleeping .... zzz\n");
                    sleep(5);
                }
            }
        } while(gotLogicPid == false) ;
    }

    if( relSem(sid) < 0) {
        perror("Failed to release lock.");
        exit(4);
    }

    if( tickDelay > 0) {
        alarm(calcDelay(tickDelay));
        signal(SIGALRM, alarmHandler);

    }
    rc = mosquitto_loop_forever(mosq, -1, 1);

}

