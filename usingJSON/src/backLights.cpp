#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {
    bool verbose=true;

    alarmBase *sunState = new alarmBase();
    alarmBase *lightsOn = new alarmBase();

    alarmBase *update = new alarmBase();

    update->setStartTime("03:25");
    update->setDuration(0);

    lightsOn->setStartTime("16:30");
    lightsOn->setEndTime("23:30");

    plcSocket *plc = new plcSocket();

    string sunRise ;
    string sunSet ;

    cout << sunRise << endl ;
    cout << sunSet << endl ;

    cout << sunState->checkTime();


    /*   sunState       16:30-23:30            BackLights
     * ----]/[--------------] [--------------------()
     */


    cout << endl;

    bool tState=false;
    int count=0;
    bool firstTime = true;

    while( true ) {
        if( update->checkTime() || firstTime) {
            firstTime=false;
            cout << "Update sun rise & set" << endl;
            sunRise = plc->getValue("SUNRISE");
            sunSet = plc->getValue("SUNSET");

            sunState->setStartTime(sunRise);
            sunState->setEndTime(sunSet);
        }

        plc->Push(sunState->checkTime());
        plc->Invert();

        plc->Push(lightsOn->checkTime());

        plc->And();

        count++;
        if( verbose ) {
        	cout << "======" << endl; ;
        	cout << "Count : "<< count << endl;
        	cout << "Stack " ;
        	plc->displayStack();
        	cout << endl;
        }

        plc->Out("BACK_FLOODLIGHTS");

        plc->dump();

        plc->End(30000 );
    }
}

