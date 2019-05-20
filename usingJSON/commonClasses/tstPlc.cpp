#include <plcBase.h>
#include <plcSocket.h>

#include <iostream>

using namespace std;

int main() {
    plcSocket *plc = new plcSocket();

    plc->dump();

    plc->Ld("START");
    plc->Ld("MOTOR");
    plc->Or();
    plc->Ld("STOP");
    plc->Invert();
    plc->And();
}

