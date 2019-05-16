/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: timerBase.cpp
 *   DATE: Wed Feb 13 14:53:46 2019
 *  DESCR: 
 ***********************************************************************/
#include "timerBase.h"
#include <sys/time.h>
#include <cstddef>

/***********************************************************************
 *  Method: msTimer::start
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void msTimer::start() {
    struct timeval startTV;
    gettimeofday(&startTV, NULL);

    startTime = (startTV.tv_sec * 1000) + (startTV.tv_usec / 1000);
}

/***********************************************************************
 *  Method: msTimer::read
 *  Params: 
 * Returns: uint32_t
 * Effects: 
 ***********************************************************************/
uint32_t msTimer::read() {
    struct timeval nowTV;

    gettimeofday(&nowTV, NULL);

    int nowTime = (nowTV.tv_sec * 1000) + (nowTV.tv_usec / 1000);

    return( nowTime - startTime );

}

