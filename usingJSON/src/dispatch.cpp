#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <unistd.h>
#include <mqueue.h>
#include <mosquitto.h>

#include <iostream>
#include <fstream>

#include <string>

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

void usage() {
    printf("Usage: dispatch -h | -c <cfg> -v \n\n");

       printf("\t-c <cfg>\tUse config file.\n");
//       printf("\t-d\t\tDisplay received message.\n");
       printf("\t-h\t\tHelp.\n");
       printf("\t-v\t\tVerbose.\n");
//       printf("\t-l\t\tLoop around for next message(s).\n");
//       printf("\t-n <name>\tThe name of the queue to open/create.\n");
//       printf("\t-s nn \t\tMessage size.\n");
//       printf("\t-t\t\tMessage is text.\n");
}

int main(int argc,char *argv[]) {
    int opt;
    bool mkPipe=false;
    bool runFlag=false;
    bool dumpMsg=false;
    bool textFlag=false;
    bool verbose=false;

    string cfgFile = "/etc/mqtt/bridge.json";

    json config ;

    while ((opt = getopt(argc, argv, "hc:v")) != -1) {
        switch(opt) {
            case 'h':
                usage();
                exit(1);
                break;
            case 'c':
                cfgFile = optarg;
                break;
            case 'v':
                verbose=true;
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

//    cout << ">" + mosquittoHost + "<" << endl;
//    cout << config["local"]["port"] << endl;

    int rc;
    int keepalive=0;
    struct mosquitto *mosq = mosquitto_new("dispatch", true, NULL);

    if( mosq) {
        rc=mosquitto_connect(mosq, mosquittoHost.c_str(), mosquittoPort, keepalive);
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

    //    char qName[255];
    string qName = "/toDispatcher";

//    cout << qName << endl;

    int msgSize=255;

    mq=mq_open(qName.c_str(), O_RDONLY,0644,&attr);
    //    mq=mq_open(qName.c_str(), O_RDONLY);

    if(mq == -1) {
        perror("mq_open");
        exit(1);
    }

    runFlag = true;

    json inJson;
    string type;

    do {
        bzero(msg,msgSize);
        rc = mq_receive(mq, msg, msgSize, 0);

        if ( rc < 0) {
            perror("mq_receive");
            runFlag=false;
        } else {
            if(verbose) {
                printf("%d bytes read\n",rc);
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
            }

        }
    } while(runFlag);

    mq_close(mq);

}
