#include <plcBase.h>
#include <plcSocket.h>

#include <iostream>

using namespace std;

int main() {
    plcSocket *plc = new plcSocket();

    plc->dump();

//    string v=plc->getValue("START");
    bool v=plc->getBoolValue("START");

//    cout << "START = " + v << endl;

    plc->setValue("START", "ON");
}

