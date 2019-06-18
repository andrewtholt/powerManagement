#include <plcBase.h>
#include <plcSocket.h>
#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    alarmBase *t1 = new alarmBase();
    alarmBase *t2 = new alarmBase();

    plcSocket *plc = new plcSocket();

    string sunRise = plc->getValue("SUNRISE");
    string sunSet = plc->getValue("SUNSET");

    cout << sunRise << endl ;
    cout << sunSet << endl ;

    t1->setStartTime(sunRise);
    t1->setEndTime(sunSet);

//    t1->setStartTime("10:15");
//    t1->setEndTime("10:37");

    cout << t1->checkTime();


    /*    04:30-23:30   DAY
     * ----][------------()
     */


    cout << endl;

    bool tState=false;
    int count=0;

    while( true ) {
        plc->Push(t1->checkTime());

        tState = t1->checkTime();

        plc->Invert();

        count++;
        cout << "======" << endl; ;
        cout << "Count : "<< count << endl;
        cout << "Stack " ;
        plc->displayStack();
        cout << endl;

        plc->Out("MOTOR");

        plc->dump();

        plc->End(30000 );
    }
}

