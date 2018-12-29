
#include "plcMQTT.h"
#include <iostream>
#include <unistd.h>


using namespace std;

void logicTst(plcMQTT *l) {
    static int count=1;
    
    cout << "LOGIC:" << count++ << endl ;
    
    l->Ld("START");
    l->Or("MOTOR");
    l->Andn("STOP");
    l->Out("MOTOR");
    
    l->plcEnd(0);

    sleep(1);
}

/*
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
    l->TimAndn("23:00");
    l->Out("BACK-LIGHT-COIL");
    
    l->plcEnd(0);
}
*/

int main(int argc, char *argv[]) {
    bool setNameFlag = true;
    bool verbose = false;
    int opt;

    while(( opt = getopt(argc, argv, "hv")) != -1) {
        switch( opt ) {
            case 'v':
                verbose=true;
                break;
            case 'h':
                printf("Help\n\n");
                exit(1);
                break;
            default:
                printf("What ?\n\n");
                exit(1);
                break;

        }
    }
    
    plcMQTT *me = new plcMQTT();

    cout << me->getDBname() << endl ;
    
    me->setLogic( NULL );

    if( verbose ) {
        cout << "Verbose" << endl;
    }
    me->setVerbose( verbose );
    
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
        exit(1);
    } else {
        printf("... OK\n");
        me->plcDump();
    }
    printf("=================\n");
    me->initPlc("tstPlc");
    
    //
    // Env, time day, sunrise etc
    // 
    // Outputs
    // 
    // 
    // Inputs
    // 
    me->addIOPoint("TIME",  "/test/time","IN");
    me->addIOPoint("START", "/test/start","IN");
    me->addIOPoint("STOP",  "/test/stop", "IN");
    me->addIOPoint("MOTOR");

    me->plcRun();
    while(1) {
        logicTst(me);
        cout << "Round and ..." << endl;
    }
}

