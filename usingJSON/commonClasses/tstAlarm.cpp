#include "alarmBase.h"
#include <unistd.h>

#include <iostream>

using namespace std;

void usage() {
    cout << "Usage: tstAlarm -h| -d <duration> -v -i -s <start time> -e <end-time>" << endl;
    cout <<"\t-h\t\tHelp" << endl;
    cout <<"\t-d <duration>\tDuration, in minutes.  If 0 output is true first time it's checked." << endl;
    cout <<"\t-v\t\tVerbose." << endl;
    cout <<"\t-i\t\tInvert.  State goes false between start and end time." << endl;
    cout <<"\t-s <start>\tStart time, format hh:mm" << endl;
    cout <<"\t-e <end>\tEnd time, format hh:mm" << endl;

}

int main(int argc, char *argv[]) {

    string alarmTime;
    int duration=0;
    
    bool verbose;
    bool invert=false;

    string startTime="";
    string endTime="";

    int opt;

    while((opt = getopt(argc, argv, "d:hvis:e:")) != -1) {
        switch( opt ) {
            case 'd':
                duration = atoi(optarg);
                break;
            case 'h':
                usage();
                exit(1);
                break;
            case 'v':
                verbose=false;
                break;
            case 's':
                startTime=optarg;
                break;
            case 'e':
                endTime=optarg;
                break;
            case 'i':
                invert=true;
                break;
        }
    }

    if( startTime == "" ) {
        cerr << "Need start time." << endl;
        usage();
        exit(1);
    }

    alarmBase *t = new alarmBase();

    cout << "Unset " << t->checkTime() << endl;;

    t->setStartTime(startTime);

    cout << "Set time " + startTime + "\t" << t->checkTime() << endl;;

    if( endTime == "" ) {
        t->setDuration(duration);
        cout << "Set duration " + to_string(duration) << endl;;
    } else {
        endTime = endTime ;
        t->setEndTime(endTime);

        cout << "Set end time " + endTime + "\t" << t->checkTime() << endl;;
    }

    t->setDow(ALL);

    t->dump();
    cout << "Set dow " + endTime + "\t" << t->checkTime() << endl;;

    int count=0;
    bool state=false;

    while( true ) {

        cout << endl << "=================" << endl;
        count++;
        cout << "Count      " << count << endl;
        cout << "Start time " + startTime + "\t" << endl;;
        cout << "End   time " + endTime + "\t" << endl;

        state = t->checkTime() ;
        if(invert) {
            state = (state == true) ? false : true;
        }
        cout << "\tState " << state << endl;

        sleep(15);
    }

}

