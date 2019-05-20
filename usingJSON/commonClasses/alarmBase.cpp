/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: .//alarmBase.cpp
 *   DATE: Mon May 20 14:36:49 2019
 *  DESCR: 
 ***********************************************************************/
#include "alarmBase.h"
#include <time.h>
#include <boost/algorithm/string.hpp>

/***********************************************************************
 *  Method: alarmBase::alarmBase
 *  Params: 
 * Effects: 
 ***********************************************************************/
alarmBase::alarmBase() {
}


/***********************************************************************
 *  Method: alarmBase::setDow
 *  Params: dow_t day
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool alarmBase::setDow(dow_t day) {

    bool failFlag=true;

    if( day >= MONDAY && day <= ALL ) {
        failFlag = false;
        dow = day;
    } else {
        failFlag = true;
    }

    return failFlag;
}


/***********************************************************************
 *  Method: alarmBase::setStartTime
 *  Params: string Time
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool alarmBase::setStartTime(string Time) {
    bool failFlag=true;

    vector<string> results;
    results=boost::split(results, Time, [](char c){return c == ':';});

    int hh = stoi(results[0]); 
    int mm = stoi(results[1]);

    startTime =  (hh*60) + mm;

    return failFlag;
}


/***********************************************************************
 *  Method: alarmBase::setEndTime
 *  Params: string Time
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool alarmBase::setEndTime(string Time) {
    bool failFlag=true;

    vector<string> results;
    results=boost::split(results, Time, [](char c){return c == ':';});

    int hh = stoi(results[0]); 
    int mm = stoi(results[1]);

    int et =  (hh*60) + mm;

    if ( et <= startTime) {
        duration = 0;
        failFlag = true;
    } else {
        duration = et;
        failFlag = false;
    }


    return failFlag;
}


/***********************************************************************
 *  Method: alarmBase::setDuration
 *  Params: uint32_t duration
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool alarmBase::setDuration(uint32_t dur) {
    bool failFlag=true;

    duration = dur;

    failFlag = false;

    return failFlag;
}


/***********************************************************************
 *  Method: alarmBase::checkTime
 *  Params: 
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool alarmBase::checkTime() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int now =( tm.tm_hour * 60) + tm.tm_min ;

    if( duration == 0) {
        state = ( now == startTime ) ? true : false ;
    } else {
        state = ( (now >= startTime) && ( now <= (startTime + duration) ) );
    }
    return state;

}


