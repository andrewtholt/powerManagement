
#include "plcMQTT.h"
#include <iostream>
#include <unistd.h>


using namespace std;

void logicBackLight(plcMQTT *l) {
    
    static int count=1;
    static bool firstTime=true;
    if(firstTime) {
        cout << "Porch light:" << count++ << endl ;
        
        firstTime=false;
    }
    
    l->TimLd("04:30");
    
    string sunSet = l->getValue("SUNSET");
    string adjSunSet = l->addMinutes(sunSet, -30);
    l->TimOr(adjSunSet);
    
    l->Or("PORCHLIGHT-CONTACT");
    
    string sunRise = l->getValue("SUNRISE");
    string adjSunRise = l->addMinutes(sunRise, 30);
    l->TimAndn(adjSunRise);
    l->TimAndn("23:00");
    l->Out("PORCHLIGHT-COIL");
    
    l->plcEnd(0);
}

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
    } else {
        printf("... OK\n");
        me->plcDump();
    }
    printf("=================\n");

    // 
    // TODO Append pid to end of name.
    //
    me->initPlc(argv[0]);
    
    //
    // Env, time day, sunrise etc
    // 
    // Outputs
    // 
    me->addIOPoint("PORCHLIGHT-COIL",    "/home/outside/PorchLight_1/cmnd/power","OUT");
    // 
    // Inputs
    // 
    me->addIOPoint("TIME",    "/test/time","IN");
    me->addIOPoint("PORCHLIGHT-CONTACT", "/home/outside/PorchLight_1/POWER","IN");
    me->addIOPoint("SUNRISE", "/test/sunrise","IN");
    me->addIOPoint("SUNSET",  "/test/sunset", "IN");
    
    me->plcRun();
    while(1) {
        logicBackLight(me);
    }
}

