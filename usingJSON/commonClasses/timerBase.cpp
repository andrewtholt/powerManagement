/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: timerBase.cpp
 *   DATE: Wed Feb 13 14:53:46 2019
 *  DESCR: 
 ***********************************************************************/
#include "timerBase.h"
#include <sys/time.h>
#include <cstddef>

msTimer::msTimer(int ms) {
    duration = ms;
}

/***********************************************************************
 *  Method: msTimer::start
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void msTimer::timerStart(void) {
    struct timeval startTV;
    gettimeofday(&startTV, NULL);

    startTime = (startTV.tv_sec * 1000) + (startTV.tv_usec / 1000);
}

bool msTimer::run(bool runFlag) {
    if(runFlag == false) {
        running=false;
        output = false;
    } else {
        if( running == true) {
            int time = read();
            if( time > duration ) {
                output=true;
            } else {
                output=false;
            }
        } else { 
            // Running is false
            // 
            timerStart();
            running = true;
            output=false;
        }
    }
    return output;
}

/***********************************************************************
 *  Method: msTimer::read
 *  Params: 
 * Returns: uint32_t
 * Effects: 
 ***********************************************************************/
int32_t msTimer::read() {
    struct timeval nowTV;

    gettimeofday(&nowTV, NULL);

    int nowTime = (nowTV.tv_sec * 1000) + (nowTV.tv_usec / 1000);

    return( nowTime - startTime );

}
