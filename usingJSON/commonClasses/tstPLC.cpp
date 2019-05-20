#include <plcSocket.h>
#include <iostream>

using namespace std;

int main() {
        cout << "tstPlc" << endl;
        
        plcSocket *plc = new plcSocket();
        
        plc->dump();
}
