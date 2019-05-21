#include "alarmBase.h"

#include <iostream>

using namespace std;

int main() {

    string alarmTime;
    int duration=0;
    string endTime;

    alarmBase *t = new alarmBase();

    cout << "Unset " << t->checkTime() << endl;;

    alarmTime="08:30";
    t->setStartTime(alarmTime);

    cout << "Set time " + alarmTime + "\t" << t->checkTime() << endl;;

    duration=10;
    t->setDuration(duration);
    cout << "Set duration " + to_string(duration) + "\t" << t->checkTime() << endl;;

    endTime = "08:56";
    t->setEndTime(endTime);

    t->dump();
    cout << "Set end time " + endTime + "\t" << t->checkTime() << endl;;

}

