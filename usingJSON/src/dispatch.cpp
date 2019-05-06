#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <unistd.h>
#include <mqueue.h>

#include <iostream>
#include <string>

using namespace std;

void usage() {
    printf("Usage: dispatch \n\n");

    /*
       printf("\t-c\t\tCreate queue\n");
       printf("\t-d\t\tDisplay received message.\n");
       printf("\t-h\t\tHelp.\n");
       printf("\t-l\t\tLoop around for next message(s).\n");
       printf("\t-n <name>\tThe name of the queue to open/create.\n");
       printf("\t-s nn \t\tMessage size.\n");
       printf("\t-t\t\tMessage is text.\n");
       */

}

int main(int argc,char *argv[]) {
    int opt;
    bool mkPipe=false;
    bool runFlag=false;
    bool dumpMsg=false;
    bool textFlag=false;

    struct mq_attr attr;

    memset(&attr, 0, sizeof attr);
    //max size of a message
    attr.mq_msgsize = 255;  //MSG_SIZE = 4096
    attr.mq_flags = 0;
    //maximum of messages on queue
    attr.mq_maxmsg = 10;

    mqd_t mq;

    int rc=0;
    char msg[255];

    //    char qName[255];
    string qName = "/toDispatcher";

    cout << qName << endl;

    int msgSize=255;

    /*
       while((opt = getopt(argc,argv,"cdhln:s:t")) != -1 ) {
       switch (opt) {
       case 'c':
       mkPipe=true;
       break;
       case 'd':
       dumpMsg=true;
       break;
       case 'h':
       usage();
       exit(0);
       break;
       case 'l':
       runFlag=true;
       break;
       case 'n':
       strncpy(qName,optarg,sizeof(qName));
       break;
       case 's':
       msgSize=atoi(optarg);
       break;
       case 't':
       textFlag=true;
       break;
       default:
       usage();
       exit(1);
       break;
       }
       }

       if(strlen(qName) == 0) {
       usage();
       exit(2);
       }

       if(mkPipe) {
       attr.mq_flags=0;
       attr.mq_maxmsg=10;
       attr.mq_msgsize=msgSize;
       mq=mq_open(qName, (O_RDONLY|O_CREAT),0644,&attr);
       } else {
       }
       */
    printf("-%s\n", qName.c_str());
    mq=mq_open(qName.c_str(), O_RDONLY,0644,&attr);
    //    mq=mq_open(qName.c_str(), O_RDONLY);

    if(mq == -1) {
        perror("mq_open");
        exit(1);
    }

    runFlag = true;

    do {
        bzero(msg,msgSize);
        rc = mq_receive(mq, msg, msgSize, 0);

        if ( rc < 0) {
            perror("mq_receive");
            runFlag=false;
        } else {
            printf("%d bytes read\n",rc);
            printf("%s\n", msg);
        }
    } while(runFlag);

    mq_close(mq);

}

