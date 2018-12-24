
#include "plcMQTT.h"
#include <iostream>
#include <unistd.h>


using namespace std;

void logicTst(plcMQTT *l) {
    static int count=1;
    
    cout << "LOGIC:" << count++ << endl ;
    
    //    l->TimLd("SUNRISE");
    l->Ld("START");
    l->Or("BACK-LIGHT-CONTACT");
    l->Andn("STOP");
    l->Out("BACK-LIGHT-COIL");
    
    /*
     *    l->Ld("START");
     *    l->TimAndn("15:34");
     *    l->Out("MOTOR");
     */
    
    l->plcEnd(0);
}

void logicBackLight(plcMQTT *l) {
    
    static int count=1;
    static bool firstTime=true;
    if(firstTime) {
        cout << "Backlight:" << count++ << endl ;
        
        firstTime=false;
    }
    
    l->TimLd("04:30");
    
    string sunSet = l->getValue("SUNSET");
    string adjSunSet = l->addMinutes(sunSet, -30);
    l->TimOr(adjSunSet);
    
    l->Or("BACK-LIGHT-CONTACT");
    
    string sunRise = l->getValue("SUNRISE");
    string adjSunRise = l->addMinutes(sunRise, 30);
    l->TimAndn(adjSunRise);
    l->TimAndn("11:00");
    l->Out("BACK-LIGHT-COIL");
    
    
    l->plcEnd(0);
}

int main() {
    bool setNameFlag = true;
    
    plcMQTT *me = new plcMQTT();
    
    me->setLogic( NULL );
    me->setVerbose(true);
    
    me->plcDump();  // Show defaults
    
    printf("=================\n");
    printf("Set name ...\n");
    setNameFlag = me->setHost("192.168.0.65");  // Change it
    if( setNameFlag ) {
        fprintf(stderr,"... Failed\n");
        exit(1);
    } else {
        printf("... OK\n");
        me->plcDump(); // Show change
    }
    
    bool setPortFlag = true;
    
    printf("=================\n");
    printf("Set port ...\n");
    setPortFlag = me->setPort(1883);
    if ( setPortFlag ) {
        printf("... Failed\n");
        fprintf(stderr,"... Failed\n");
    } else {
        printf("... OK\n");
        me->plcDump();
    }
    printf("=================\n");
    
    me->initPlc("tstPlc");
    
    // Env, time day, sunrise etc
    /*
     *    me->addIOPoint("DAY",     "/test/day","IN");
     *    me->addIOPoint("SUNRISE", "/test/sunrise","IN");
     *    me->addIOPoint("SUNSET",  "/test/sunset","IN");
     */
    
    // Tests
    /*
     *    me->addIOPoint("BACK-LIGHT-COIL",    "/home/outside/BackFloodlight/cmnd/power","OUT");
     *    
     *    me->addIOPoint("BACK-LIGHT-CONTACT", "/home/outside/BackFloodlight/POWER","IN");
     *    me->addIOPoint("START", "/test/start","IN");
     *    me->addIOPoint("STOP", "/test/stop","IN");
     */
    
    // 
    // Outputs
    // 
    me->addIOPoint("BACK-LIGHT-COIL",    "/home/outside/BackFloodlight/cmnd/power","OUT");
    // 
    // Inputs
    // 
    me->addIOPoint("TIME",    "/test/time","IN");
    me->addIOPoint("BACK-LIGHT-CONTACT", "/home/outside/BackFloodlight/POWER","IN");
    me->addIOPoint("SUNRISE", "/test/sunrise","IN");
    me->addIOPoint("SUNSET",  "/test/sunset", "IN");
    me->addIOPoint("TIME",  "/test/time", "IN");
    
    me->plcRun();
    while(1) {
//        logicTst(me);
        logicBackLight(me);
        sleep(1);
    }
}

