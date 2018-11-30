
#include "plcMQTT.h"
#include <iostream>

using namespace std;

void logic(plcMQTT *l) {
    cout << "LOGIC" << endl ;

    l->Ld("BACK_LIGHT");
    l->Outn("fred");
}

int main() {
    bool setNameFlag = true;

    plcMQTT *me = new plcMQTT();

    me->setLogic( logic );

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

    me->addIOPoint("fred");

    me->addIOPoint("BACK_LIGHT", "/home/outside/BackFloodlight/cmnd/power","IN");

    string result = me->getValue( "BACK_LIGHT" );

    bool setValueFlag = me->setValue("BACK_LIGHT","TEST_VALUE");

    cout << result << endl;

    me->plcRun();
}

