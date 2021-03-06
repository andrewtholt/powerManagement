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

    plc->setVerbose(true);
    plc->dump();

    while( 1 ) {
        plc->Ld("START");

        plc->Ld("MOTOR");

        plc->Or();

        plc->Ld("STOP");

        plc->Invert();

        plc->And();

        plc->Out("MOTOR");
        plc->End( 150 );
    }
}

