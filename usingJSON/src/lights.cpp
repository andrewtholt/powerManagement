#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    alarmBase *t1 = new alarmBase();
    alarmBase *t2 = new alarmBase();

    plcSocket *plc = new plcSocket();

    cout << t1->checkTime();

    t1->setStartTime("04:30");
    t2->setStartTime("07:30");

    plc->dump();

    /*  04:30      07:30   MORNING
     * --][------+--]/[-----()
     *           |
     * MORNING   |
     * --][------+
     */


    while( true ) {
        plc->Ld("MORNING");
        plc->Push(t1->checkTime());

        plc->displayStack();

        plc->Or();
        plc->Push(t2->checkTime());
        plc->Invert();
        plc->And();
        plc->Out("MORNING");
        /*

           plc->Ld("START");
           plc->Ld("MOTOR");
           plc->Or();
           plc->Ld("STOP");
           plc->Invert();
           plc->And();
           plc->End( 150 );
           */
        plc->End(60000 );
    }
}

