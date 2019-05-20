/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: .//alarmBase.cpp
 *   DATE: Mon May 20 14:36:49 2019
 *  DESCR: 
 ***********************************************************************/
#include "alarmBase.h"
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

    return failFlag;
}


/***********************************************************************
 *  Method: alarmBase::setDuration
 *  Params: uint32_t duration
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool alarmBase::setDuration(uint32_t duration) {
    bool failFlag=true;

    return failFlag;
}


