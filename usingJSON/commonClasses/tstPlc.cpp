#include <plcBase.h>
#include <plcSocket.h>

#include <iostream>

using namespace std;

int main() {
    plcSocket *plc = new plcSocket();

    plc->dump();

    plc->getValue("START");
}

