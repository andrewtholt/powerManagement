#include "myClientClass.h"
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>
#include <sys/types.h>
#include <signal.h>
#include <hspread.h>

#include <iostream>
#include <string>
#include <map>

// #define mqtt_host "127.0.0.1"
#define mqtt_host "192.168.0.65"
#define mqtt_port 1883

using namespace std;
int logicPid=0;
bool runFlag;
string srcGroup = "output";

enum ioType {
    INVALID=0,
    INPUT,
    OUTPUT,
    INTERNAL,
};

struct ioPointCache {
    bool value;
    ioType type;
    string onState;
    string offState;
    string topic;
};

map<string, struct ioPointCache> localIoPoint;

void alarmHandler(int sig) {
    signal(SIGUSR1, SIG_IGN);
    printf("Alarm fired\n");
    signal(SIGUSR1, alarmHandler);
}

void hupHandler(int sig) {
    signal(SIGHUP, SIG_IGN);
    printf("HUP fired\n");
    runFlag=false;
    signal(SIGHUP, hupHandler);
}
//
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
    printf("\t-g <spread group>\tSpread group to send message to\n");
    printf("\t-h|?\t\tHelp\n");
    printf("\t-i <name>\tMy name for spread\n");
    printf("\t-n <name>\tDatabase hostname\n");
    printf("\t-p <svc name>\tDatabase service name\n");
    printf("\t-s <spread host>\tHostname of spread host\n");
    printf("\t-v\t\tVerbose\n");
}

int main(int argc, char *argv[]) {
    bool verbose=false;
    
    int len=0;
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];
    
    string hostName = "localhost";
    string serviceName ="myclient" ;
    string spreadHost = "localhost" ;
    string myName = "updateOutputsMQTT";
    int spreadPort=4803;
    
    int opt;
    int rc;
    
    while (( opt = getopt(argc, argv, "g:h?n:p:s:v")) !=-1) {
        switch(opt) {
            case 'g':
                // Spread group to receive messages from.
                srcGroup = optarg;
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
            case 'v':
                verbose = true;
                break;
        }
    }
    
    if(verbose) {
        cout << "Hostname : " << hostName << endl;
        cout << "Service  : " << serviceName << endl;
        
        cout << "Spread Host : " << spreadHost << endl;
        cout << "I am        : " << myName << endl;
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
    
    usleep(100 * 1000);
    while ( n->clientConnected() == false ) {
        if( verbose ) {
            fprintf(stderr, "FATAL ERROR: Connected to interface but not to database\n");
        }
        sleep(2);
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
    
    while(true) {
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
        
        //        signal(SIGUSR1, alarmHandler);
        //        signal(SIGHUP, hupHandler);
        
        
        //
        // Setup spread
        //
        char rxMsg[255];
        
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
        rc=SPJoinSimple((char *)srcGroup.c_str());
        
        runFlag = true;
        string out;
        
        char sql[512];
        
        string name;
        string state;
        string onState;
        string offState;
        string outputState;
        string direction;
        bool selectFailed = true;
        int cnt ;
        string cmd ;
        
        do {
            sprintf(sql, "select name,topic,state,on_state, off_state,direction  from io_point where direction = 'OUT' or direction = 'INTERNAL';\n");
            len = n->sendCmd( sql );
            
            cmd = "^get-row-count\n";
            len = n->sendCmd( cmd, out );
            
            string::size_type sz;   // alias of size_t
            
            cout << out << endl;
            
            try {
                cnt = stoi( out,&sz);
                selectFailed=false;
            } 
            catch (...) {
                cout << "stoi exception" << endl;
                sleep(1);
                selectFailed=true;;
            }
        } while(selectFailed);
        
        for( int i=0; i < cnt ; i++ ) {
            if( i > 0 ) {
                cmd = "^go-next\n";
                len = n->sendCmd( cmd );
            }
            cmd = "^get-col name\n";
            len = n->sendCmd( cmd, name );
            
            cmd = "^get-col topic\n";
            len = n->sendCmd( cmd, topic );
            
            cmd = "^get-col direction\n";
            len = n->sendCmd( cmd, direction );
            
            cmd = "^get-col on_state\n";
            len = n->sendCmd( cmd, onState );
            
            cmd = "^get-col off_state\n";
            len = n->sendCmd( cmd, offState );
            
            localIoPoint[name].value = false;
            localIoPoint[name].onState = onState;
            localIoPoint[name].offState = offState;
            localIoPoint[name].topic = topic;
            
            if(direction == "OUT") {
                localIoPoint[name].type = OUTPUT;
            } else {
                localIoPoint[name].type = INTERNAL;
            }
            
            if(verbose) {
                printf("%s\n\t%d\n\t%s\n", name.c_str() , 
                       localIoPoint[name].type,
                       localIoPoint[name].topic.c_str() );
            }
        }
        
        
        string res;
        while(runFlag) {
            rc= SPRxSimple( rxMsg, 255) ;
            int len=strlen(rxMsg);
            if ( len > 0) {
                len--;
                if( isalnum(rxMsg[len]) == 0 ) {
                    rxMsg[len]='\0';
                }
                name = strtok(rxMsg," ");
                state = strtok(NULL," \n");
                
                if(localIoPoint.find( name ) != localIoPoint.end() ) {
                    if ( state == "TRUE" ) {
                        outputState = localIoPoint[name].onState;
                        localIoPoint[name].value = true;
                    } else if ( state == "FALSE" ) {
                        outputState = localIoPoint[name].offState;
                        localIoPoint[name].value = false;
                    } else {
                        outputState = "UNKNOWN";
                    }
                    if(verbose) {
                        cout << "Name : " + name << endl;
                        cout << "OutputState :" + outputState << endl;
                        cout << "topic       :" + localIoPoint[name].topic << endl;
                    } 
                } else {
                    localIoPoint[name].topic = "/home/internal/" + name;
                    localIoPoint[name].onState = "TRUE";
                    localIoPoint[name].offState = "FALSE";
                    outputState = state;
                    
                    sprintf(sql, "insert into io_point VALUES  ('%s','%s','INTERNAL','%s','TRUE','FALSE')\n", (char *)name.c_str(), 
                            localIoPoint[name].topic.c_str(),
                            (char *)outputState.c_str() );
                    
                    if(verbose) {
                        printf("%s\n", sql);
                    }
                    
                    len = n->sendCmd( sql );
                }
                mosquitto_publish(mosq, NULL, localIoPoint[name].topic.c_str(), outputState.length(), outputState.c_str(),0,true);
            } 
        }
    }
}

