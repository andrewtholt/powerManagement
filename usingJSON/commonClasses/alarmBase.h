#include <stdint.h>
#include <iostream>
#include <vector>
#include <strings.h>
#include <string.h>
// #include <boost/algorithm/string.hpp>

/*
    When an instance is created the times are set to wildcards, so the state
    is true, since it matches all times.
*/
enum dow_t { 
    SUNDAY=0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    WORKDAY, // MON-FRI
    WEEKEND,  // SAT,SUN
    ALL
};

using namespace std;

class alarmBase {
    private:
        int32_t startTime=-1;
        uint32_t duration=0;
        int32_t dom=-1;
        int32_t mon=-1;
        int32_t year=-1;
        bool state = true;

        bool triggered = false; // if duration is set to 0, and now is start time then set
                                // state true, and this too.
                                // Next pass, this will be true, so set to false.
                                // When we are pass the start time set this false.

        dow_t dow=ALL;
    public:
        alarmBase();
        bool setDow(dow_t day);
        bool setStartTime(string Time); // Time is hh:mm
        bool setEndTime(string Time); // Time is hh:mm
        bool setDuration(uint32_t duration); // in minutes.

        void adjStartTime(int d);
        void adjEndTime(int d);

        bool checkTime();

        void dump();
};


