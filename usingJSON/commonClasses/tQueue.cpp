/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//tQueue.cpp
 *   DATE: Mon Jul 29 09:43:48 2019
 *  DESCR: 
 ***********************************************************************/
#include "tQueue.h"

using namespace std;

/***********************************************************************
 *  Method: tQueue::push
 *  Params: string msg
 * Effects: 
 ***********************************************************************/
void tQueue::push(string msg) {
    mtx.lock();
    msgQueue.push(msg);
    mtx.unlock();
}


/***********************************************************************
 *  Method: tQueue::pop
 *  Params: 
 * Returns: string
 * Effects: 
 ***********************************************************************/
string tQueue::pop() {
    string out;
    mtx.lock();
    out = msgQueue.front();
    msgQueue.pop();
    mtx.unlock();
    return out;
}


/***********************************************************************
 *  Method: tQueue::get
 *  Params: 
 * Returns: string
 * Effects: 
 ***********************************************************************/
string tQueue::get() {
    string out;
    mtx.lock();
    out = msgQueue.front();
    mtx.unlock();
    return out;
}


/***********************************************************************
 *  Method: tQueue::depth
 *  Params: 
 * Returns: int
 * Effects: 
 ***********************************************************************/
int tQueue::depth() {
    int s=0;
    mtx.lock();
    s=msgQueue.size();
    mtx.unlock();
    return s;
}

/***********************************************************************
 *  Method: tQueue::discard
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void tQueue::discard() {
    mtx.lock();
    msgQueue.pop();
    mtx.unlock();
}


