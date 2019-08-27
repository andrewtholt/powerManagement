#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    plcSocket *plc = new plcSocket();

    bool failFlag = plc->getStatus();

    if(failFlag) {
        cerr << "FATAL ERROR: Instanciation of PLC socket failed." << endl;
        exit(1);
    }
    alarmBase *t = new alarmBase();

    cout << t->checkTime();

    t->setStartTime("05:30");

    plc->dump();

    plc->Ld("START");
    plc->Ld("MOTOR");
    plc->Or();
    plc->Ld("STOP");
    plc->Invert();
    plc->And();
}

