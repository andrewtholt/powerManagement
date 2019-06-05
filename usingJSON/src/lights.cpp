#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    alarmBase *t1 = new alarmBase();
    alarmBase *t2 = new alarmBase();

    plcSocket *plc = new plcSocket();

    cout << plc->getValue("SUNRISE");
    cout << t1->checkTime();

    t1->setStartTime("04:30");
    t1->setEndTime("23:30");

    plc->dump();

    /*  04:30     17:30    DAY
     * --][--------]/[-----()
     */


    while( true ) {
        plc->Push(t1->checkTime());

        plc->Invert();
        plc->displayStack();

        plc->Out("MOTOR");
        plc->End(30000 );
    }
}

