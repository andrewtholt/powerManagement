/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: .//alarmBase.cpp
 *   DATE: Mon May 20 14:36:49 2019
 *  DESCR: 
 ***********************************************************************/
#include "alarmBase.h"
#include <time.h>

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

    char buffer[16];
    char *ptr;
    char *tok;
    char *rest = buffer;

    int hh, mm;

    bzero(buffer,sizeof(buffer));

    strcpy( buffer, Time.c_str() ) ;

    tok = strtok_r(rest,":", &rest);
    hh = atoi(tok);

    tok = strtok_r(rest,":", &rest);
    mm = atoi(tok);

    /*
    vector<string> results;
    results=boost::split(results, Time, [](char c){return c == ':';});

    int hh = stoi(results[0]); 
    int mm = stoi(results[1]);
    */

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
        duration = et - startTime;
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
    bool run=false;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int now =( tm.tm_hour * 60) + tm.tm_min ;

    if ( startTime < 0 && dow == ALL ) {
        state=true;
    } else {
        int today=tm.tm_wday;

        switch( dow ) {
            case SUNDAY:
                if( today == 0) {
                    run = true;
                }
                break;
            case MONDAY:
                if( today == 1) {
                    run = true;
                }
                break;
            case TUESDAY:
                if( today == 2) {
                    run = true;
                }
                break;
            case WEDNESDAY:
                if( today == 3) {
                    run = true;
                }
                break;
            case THURSDAY:
                if( today == 4) {
                    run = true;
                }
                break;
            case FRIDAY:
                if( today == 5) {
                    run = true;
                }
                break;
            case SATURDAY:
                if( today == 6) {
                    run = true;
                }
                break;
            case WORKDAY:
                if( today == 0 || today == 6 ) {
                    run = true;
                }
                break;
            case WEEKEND:
                if (today > 0 && today < 6) {
                    run = true;
                }
                break;
            case ALL:
                run=true;
                break;
            default:
                run=false;
                break;
        }

        if( run ) {
            if( duration == 0) {
                state = ( now == startTime ) ? true : false ;
            } else {
                state = ( (now >= startTime) && ( now <= (startTime + duration) ) );
            }
        } else {
            state=false;
        }
    }
    return state;
}


/***********************************************************************
 *  Method: alarmBase::dump
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void alarmBase::dump() {
    cout << "Start Time : " + startTime << endl;
    cout << "Duration   : " + to_string(duration) << endl;
    cout << "State      : " + to_string(state) << endl;
}


/***********************************************************************
 *  Method: alarmBase::adjStartTime
 *  Params: int d
 * Returns: void
 * Effects: 
 ***********************************************************************/
void alarmBase::adjStartTime(int d) {
    int tmp = startTime ;
    tmp += d;

    if( d < 0 ) {
        d = 0;
    } else if ( d > (( 23 *60 ) + 59 ) ) {
        d = (( 23 *60 ) + 59 ) ;
    }

    startTime = d;
}


/***********************************************************************
 *  Method: alarmBase::adjEndTime
 *  Params: int d
 * Returns: void
 * Effects: 
 ***********************************************************************/
void alarmBase::adjEndTime(int d) {
    int tmp = duration;

    tmp += d;

    if ( tmp < 0) {
        tmp = 0;
    }

    duration = tmp;
}


