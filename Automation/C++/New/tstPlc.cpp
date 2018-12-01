
#include "plcMQTT.h"
#include <iostream>

using namespace std;

void logic(plcMQTT *l) {
    static int count=1;

    cout << "LOGIC:" << count++ << endl ;

    l->Ld("START");
    l->Or("MOTOR");
    l->Andn("STOP");
    l->Out("MOTOR");
}

int main() {
    bool setNameFlag = true;

    plcMQTT *me = new plcMQTT();

    me->setLogic( logic );
//    me->setVerbose(true);

    me->plcDump();  // Show defaults

    printf("=================\n");
    printf("Change name ...\n");
    setNameFlag = me->setHost("192.168.0.65");  // Change it
    if( setNameFlag ) {
        printf("... Failed\n");
    } else {
        printf("... OK\n");
        me->plcDump(); // Show change
    }


    printf("=================\n");
    printf("Change name again ...\n");

    setNameFlag = me->setHost("192.168.0.99");  // Change it
    if( setNameFlag ) {
        printf("... Failed\n");
    } else {
        printf("... OK\n");
    }

    me->plcDump(); // Show it hasn't changed

    bool setPortFlag = true;

    printf("=================\n");
    printf("Set port ...\n");
    setPortFlag = me->setPort(1883);
    if ( setPortFlag ) {
        printf("... Failed\n");
    } else {
        printf("... OK\n");
        me->plcDump();
    }
    printf("=================\n");

    me->initPlc();

    me->addIOPoint("TIME", "/test/time","IN");

    me->addIOPoint("START", "/test/start","IN");
    me->addIOPoint("STOP", "/test/stop","IN");
    me->addIOPoint("MOTOR", "/home/outside/BackFloodlight/cmnd/power","OUT");


    me->plcRun();
}

