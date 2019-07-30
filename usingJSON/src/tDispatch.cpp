#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include <unistd.h>
#include <mqueue.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#define TO_DISPATCHER "/toDispatcher"
#define TO_MQTT "/toMQTT"
#define TO_SNMP "/toSNMP"

#define MSG_SIZE (255)

mqd_t mkQueue(string name, int dir) {

    mqd_t mq;
    struct mq_attr *attr = (struct mq_attr *)malloc( sizeof(struct mq_attr));

    attr->mq_flags=0;
    attr->mq_maxmsg=10;
    attr->mq_msgsize=MSG_SIZE;

//    mq=mq_open(name.c_str(), (O_RDONLY|O_CREAT),0644, attr);
    mq=mq_open(name.c_str(), (dir|O_CREAT),0644, attr);

    if(mq == -1) {
        perror("mq_open");
        free(attr);
    }
    return mq;
}

int main(int argc,char *argv[]) {
    int opt;

    bool runFlag=true;
    bool dumpMsg=false;
    bool textFlag=true;

    struct mq_attr attr;
    json inJson;

    mqd_t mqDispatch;
    mqd_t mqMQTT;
    mqd_t mqSNMP;

    mqDispatch = mkQueue(TO_DISPATCHER,O_RDONLY);

    if(mqDispatch < 0) {
        perror(TO_DISPATCHER);
        exit(1);
    }

    mqMQTT = mkQueue(TO_MQTT,O_WRONLY);

    if(mqMQTT < 0) {
        perror(TO_MQTT);
        exit(1);
    }

    mqSNMP = mkQueue(TO_SNMP,O_WRONLY);

    if(mqSNMP < 0) {
        perror(TO_SNMP);
        exit(1);
    }

    char msg[MSG_SIZE];
    int rc=0;
    string type = "UNKNOWN";

    do {
        bzero(msg, MSG_SIZE);
        rc = mq_receive(mqDispatch, msg, MSG_SIZE, 0);
        if ( rc < 0) {
            perror("mq_receive");
            runFlag=false;
        } else {
            printf("%d bytes read\n",rc);
            if( textFlag) {
                printf("%s\n", msg);
            }
            inJson = json::parse( msg );

            type = inJson["type"];

            if( type == "snmp" ) {
                rc=mq_send(mqSNMP, msg, strlen(msg), 0);
                if(rc < 0) {
                    perror("SNMP");
                    cout << mqSNMP << endl;
                }
            } else if ( type == "mqtt" ) {
                rc=mq_send(mqMQTT, msg, strlen(msg), 0);
                if(rc < 0) {
                    perror("MQTT");
                }
            }

            cout << "Type : " + type << endl;
        }
    } while(runFlag);

    mq_close(mqDispatch);

}

