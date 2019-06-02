#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    alarmBase *t = new alarmBase();
    plcSocket *plc = new plcSocket();

    cout << t->checkTime();

    t->setStartTime("05:30");

    plc->dump();

    while( 1 ) {
        plc->Ld("START");
        plc->displayStack();

        plc->Ld("MOTOR");
        plc->displayStack();

        plc->Or();
        plc->displayStack();

        plc->Ld("STOP");
        plc->displayStack();

        plc->Invert();
        plc->displayStack();

        plc->And();
        plc->displayStack();

        plc->Out("MOTOR");
        plc->End( 150 );
    }
}

