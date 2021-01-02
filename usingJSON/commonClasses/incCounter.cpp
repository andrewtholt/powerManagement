/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//incCounter.cpp
 *   DATE: Sun Sep  1 10:10:54 2019
 *  DESCR: 
 ***********************************************************************/
#include "incCounter.h"

/***********************************************************************
 *  Method: incCounter::increment
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void incCounter::increment() {
    mtx.lock();
    count++;
    mtx.unlock();
}


/***********************************************************************
 *  Method: incCounter::decrement
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void incCounter::decrement() {
    mtx.lock();
    if(count > 0) {
        count--;
    }
    mtx.unlock();
}


/***********************************************************************
 *  Method: incCounter::get
 *  Params: 
 * Returns: long
 * Effects: 
 ***********************************************************************/
long incCounter::get() {
    long ret;
    mtx.lock();
    ret=count;
    mtx.unlock();
    return count;
}


/***********************************************************************
 *  Method: incCounter::reset
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void incCounter::reset() {
    mtx.lock();
    count=0;
    mtx.unlock();
}


