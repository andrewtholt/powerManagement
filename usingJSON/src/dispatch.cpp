#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <unistd.h>
#include <mqueue.h>
#include <mosquitto.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <string>

#include <nlohmann/json.hpp>

#define LOG_FILE "/tmp/dispatch.log"

using namespace std;
using json = nlohmann::json;

bool connected=false;

void on_connect(struct mosquitto *mosq, void *obj, int result) {
    connected=true;

    cerr << "on_connect called, connected" << endl;
}

void on_disconnect(struct mosquitto *mosq, void *obj, int result) {
    connected=false;

    cerr << "on_disconnect called, disconnected" << endl;
}

void usage() {
    printf("Usage: dispatch -h | -c <cfg> -v \n\n");

    printf("\t-c <cfg>\tUse config file.\n");
    printf("\t-l\t\tLog received message.\n");
    printf("\t-h\t\tHelp.\n");
    printf("\t-f\t\tRun in foreground..\n");
    printf("\t-v\t\tVerbose.\n");
    //       printf("\t-l\t\tLoop around for next message(s).\n");
    //       printf("\t-n <name>\tThe name of the queue to open/create.\n");
    //       printf("\t-s nn \t\tMessage size.\n");
    //       printf("\t-t\t\tMessage is text.\n");
}

uid_t getUserIdByName(const char *name) {
    struct passwd *pwd = getpwnam(name); /* don't free, see getpwnam() for details */
    if(pwd == NULL) {
        throw runtime_error(string("Failed to get userId from username : ") + name);
    }
    return pwd->pw_uid;
}


int main(int argc,char *argv[]) {
    int opt;
    bool mkPipe=false;
    bool runFlag=false;
    bool logMsg=false;
    bool textFlag=false;
    bool verbose=false;
    bool fg=false;

    int rc;
    int keepalive=0;

    string logFile = LOG_FILE;

    string cfgFile = "/etc/mqtt/bridge.json";

    json config ;

    ofstream outfile;


    while ((opt = getopt(argc, argv, "lhc:vf")) != -1) {
        switch(opt) {
            case 'h':
                usage();
                exit(1);
                break;
            case 'c':
                cfgFile = optarg;
                break;
            case 'f':
                fg=true;
                break;
            case 'v':
                verbose=true;
                break;
            case 'l':
                logMsg = true;
                break;
        }
    }

    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;
        exit(2);
    }

    ifstream cfgStream( cfgFile );

    config = json::parse(cfgStream);

    struct mq_attr attr;

    string mosquittoHost = config["local"]["name"];
    string mqttPortString = config["local"]["port"];

    int mosquittoPort = stoi( mqttPortString );

    bool iamRoot=false;

    if( getuid() !=0) {
        cerr << "Not root so running in the foreground\n" << endl;
        fg=true;

        iamRoot = false;
    } else {
        iamRoot = true;
    }

    memset(&attr, 0, sizeof attr);
    // 
    // Max size of a message
    // 
    attr.mq_msgsize = 255;  // MSG_SIZE = 4096
    attr.mq_flags = 0;
    // 
    // Maximum of messages on queue
    // 
    attr.mq_maxmsg = 10;

    mqd_t mq;

    char msg[255];

    string qName = "/toDispatcher";

    int msgSize=255;

    mq=mq_open(qName.c_str(), O_RDONLY,0644,&attr);

    if(mq == -1) {
        perror("mq_open");
        exit(1);
    }

    runFlag = true;

    json inJson;
    string type;

    if (fg == false ) {
        rc = daemon(true,false);

        if(iamRoot) {
            FILE *run=fopen("/var/run/dispatch.pid", "w");
            fprintf(run,"%d\n", getpid());
            fclose(run);
        }
    }

    if(iamRoot) {
        uid_t powerUser = getUserIdByName("power");
        if(setuid(powerUser) != 0) {
            perror("setuid");
            exit(1);
        }
    }

    if( logMsg) {
        outfile.open(LOG_FILE, ios_base::app);
    }


    struct mosquitto *mosq = mosquitto_new("dispatch", true, NULL);

    if( !mosq) {
        cerr << "Failed to create mosquitto." << endl;
        exit(3);
    }

        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_disconnect_callback_set(mosq, on_disconnect);
        rc=mosquitto_connect(mosq, mosquittoHost.c_str(), mosquittoPort, keepalive);

        if( rc != MOSQ_ERR_SUCCESS) {
            cerr << "mosquitto_connect error:" << rc << endl;
            perror("mosquitt_connect:");
            exit(2);
        }
//        rc=mosquitto_loop_start(mosq);

    time_t t ;
    struct tm now ;
    char timeBuff[255];
    int len = 0;

    do {
        bzero(msg,msgSize);
        len = mq_receive(mq, msg, msgSize, 0);

        if ( len < 0) {
            perror("mq_receive");
            runFlag=false;
        } else {

            if( connected == false ) {

                /*
                mosquitto_connect_callback_set(mosq, on_connect);
                mosquitto_disconnect_callback_set(mosq, on_disconnect);

                rc=mosquitto_connect(mosq, mosquittoHost.c_str(), mosquittoPort, keepalive);
//                rc=mosquitto_loop_start(mosq);

                if( rc == MOSQ_ERR_SUCCESS) {
                    connected = true;
                } else {
                    cerr << "mosquitto_loop_start error:" << rc << endl;
                    perror("mosquitt_loop_start:");
                    exit(2);
                }
                */
            }

            if (logMsg){
                bzero(timeBuff, 255);
                t = time(NULL);
                now = *localtime(&t);

                sprintf(timeBuff,"%02d:%02d:%02d - ", now.tm_hour, now.tm_min, now.tm_sec);

                outfile << timeBuff << msg << endl << flush;

            }

            if(verbose) {
                printf("%d bytes read\n",len);
                printf("%s\n", msg);
            }

            inJson = json::parse( msg );

            type = inJson["type"];

            if(verbose) {
                cout << "Type : " + type << endl;
            }

            if ( type == "mqtt" ) {
                string topic = inJson["topic"];
                string msg   = inJson["state"];

                if(verbose) {
                    cout << "Topic: " + topic << endl;
                    cout << "Msg  : " + msg << endl;
                }

                mosquitto_publish(mosq, NULL, (char *)topic.c_str(), msg.length() , (char *)msg.c_str(), 1,true) ;
                rc=mosquitto_loop(mosq,-1,1);

                if( rc != MOSQ_ERR_SUCCESS) {
                    cerr << "mosquitto_loop error:" << rc << endl;
                    perror("mosquitto_loop:");
                    sleep(10);
                    mosquitto_reconnect(mosq);
                }
            }

        }
    } while(runFlag);

    mq_close(mq);

}

