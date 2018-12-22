
#include "plcMQTT.h"
#include <iostream>
#include <unistd.h>


using namespace std;

void logicTst(plcMQTT *l) {
    static int count=1;

    cout << "LOGIC:" << count++ << endl ;

    l->TimLd("SUNRISE");
//    l->Ld("START");
    l->Or("MOTOR");
    l->Andn("STOP");
    l->Out("MOTOR");

    /*
    l->Ld("START");
    l->TimAndn("15:34");
    l->Out("MOTOR");
    */

    l->plcEnd(0);
}

void logicBackLight(plcMQTT *l) {
   
    static int count=1;
    static bool firstTime=true;
    if(firstTime) {
        cout << "Backlight:" << count++ << endl ;
        // 
        // Outputs
        // 
        l->addIOPoint("BACK-LIGHT-COIL",    "/home/outside/BackFloodlight/cmnd/power","OUT");
        // 
        // Inputs
        // 
        l->addIOPoint("BACK-LIGHT-CONTACT", "/home/outside/BackFloodlight/POWER","IN");
        l->addIOPoint("SUNRISE", "/test/sunrise","IN");
//        l->addIOPoint("START", "/test/start","IN");
//        l->addIOPoint("STOP", "/test/stop","IN");

        firstTime=false;
    }
    
    l->TimLd("04:30");
    l->Or("BACK-LIGHT-CONTACT");
    
    string sunRise = l->getValue("SUNRISE");
    string adjSunRise = l->addMinutes(sunRise, 30);
    l->TimAnd(adjSunRise);
    l->Out("BACK-LIGHT-COIL");
    
    
    l->plcEnd(0);
}


int main() {
    bool setNameFlag = true;

    plcMQTT *me = new plcMQTT();

//    me->setLogic( logic );
    me->setVerbose(true);

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

    // Env, time day, sunrise etc
    /*
    me->addIOPoint("TIME",    "/test/time","IN");
    me->addIOPoint("DAY",     "/test/day","IN");
    me->addIOPoint("SUNRISE", "/test/sunrise","IN");
    me->addIOPoint("SUNSET",  "/test/sunset","IN");

    // Tests
    me->addIOPoint("START", "/test/start","IN");
    me->addIOPoint("STOP", "/test/stop","IN");
    me->addIOPoint("MOTOR", "/home/outside/BackFloodlight/cmnd/power","OUT");
    */


    me->plcRun();
    while(1) {
//        logicTst(me);
        logicBackLight(me);
        sleep(1);
    }
}

