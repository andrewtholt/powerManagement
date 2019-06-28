#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {
    bool verbose=true;

    alarmBase *sunState = new alarmBase();
    alarmBase *lightsMorning = new alarmBase();
    alarmBase *lightsEvening = new alarmBase();

    alarmBase *update = new alarmBase();

    update->setStartTime("03:25");
    update->setDuration(0);

    lightsMorning->setStartTime("04:30");
    lightsMorning->setEndTime("07:30");

    lightsEvening->setStartTime("16:30");
    lightsEvening->setEndTime("23:30");


    plcSocket *plc = new plcSocket();

    string sunRise ;
    string sunSet ;

    cout << sunRise << endl ;
    cout << sunSet << endl ;

    cout << sunState->checkTime();

    /*   lightsEvening
     *  16:30-23:30    sunState     PorchLights
     * ----] [----------+---]/[------------()
     *                  |
     *   lightsMorning  |
     *  04:30-07:30     |
     * ----] [----------+
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

        plc->Push(lightsMorning->checkTime());
        plc->Push(lightsEvening->checkTime());
        plc->Or();

        plc->Push(sunState->checkTime());
        plc->Invert();

        plc->And();

        count++;

        if( verbose ) {
        	cout << "======" << endl; ;
        	cout << "Count : "<< count << endl;
        	cout << "Stack " ;
        	plc->displayStack();
        	cout << endl;
        }

        plc->Out("PORCH_LIGHTS");

        plc->dump();

        plc->End(30000 );
    }
}
