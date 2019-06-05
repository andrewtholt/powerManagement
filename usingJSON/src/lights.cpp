#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    alarmBase *t1 = new alarmBase();
    alarmBase *t2 = new alarmBase();

    plcSocket *plc = new plcSocket();

    string sunRise = plc->getValue("SUNRISE");
    cout << sunRise << endl ;

    t1->setStartTime(sunRise);
    t1->setEndTime("23:30");

    t1->dump();
    cout << t1->checkTime();

    plc->dump();

    /*    04:30-23:30   DAY
     * ----][------------()
     */


    while( true ) {
        plc->Push(t1->checkTime());

        plc->Invert();
        plc->displayStack();

        plc->Out("MOTOR");
        plc->End(30000 );
    }
}

