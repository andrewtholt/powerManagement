#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    alarmBase *t = new alarmBase();
    plcSocket *plc = new plcSocket();

    t->setStartTime("05:30");

    cout << t->checkTime();

    plc->dump();

    plc->Ld("START");
    plc->Ld("MOTOR");
    plc->Or();
    plc->Ld("STOP");
    plc->Invert();
    plc->And();
}

