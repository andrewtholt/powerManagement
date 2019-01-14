
#include <iostream>
#include <unistd.h>
#include "plcDatabase.h"


using namespace std;

/*
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
*/

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
    string database;
    bool nameSet=false;
    plcDatabase *me;

    while(( opt = getopt(argc, argv, "d:hv")) != -1) {
        switch( opt ) {
            case 'd':
                database = (char *)optarg ;
                nameSet = true;
                break;
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
    
    if( nameSet ) {
        me = new plcDatabase(database);
    } else {
        me = new plcDatabase();
    }

    cout << me->getDBname() << endl ;
/*    
    me->setLogic( NULL );

    if( verbose ) {
        cout << "Verbose" << endl;
    }
    */
    me->setVerbose( verbose );
    
    me->plcDump();  // Show defaults
    
    me->initPlc("tstPlc");
    /*
    
    //
    // Env, time day, sunrise etc
    // 
    // Outputs
    // 
    // 
    // Inputs
    // 
    me->addIOPoint("TIME",  "/test/time","IN");
    */
    
    me->addIOPoint("START", "switch.test_start","IN");
    me->addIOPoint("STOP",  "switch.test_stop", "IN");
    me->addIOPoint("MOTOR");

    me->Ld("START");
    me->Or("STOP");
    me->Andn("STOP");
    me->Out("MOTOR");
    /*
    me->plcRun();
    while(1) {
        logicTst(me);
        cout << "Round and ..." << endl;
    }
*/
}

