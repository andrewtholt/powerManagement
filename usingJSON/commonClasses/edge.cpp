/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//edge.cpp
 *   DATE: Wed Jul  3 09:24:09 2019
 *  DESCR: 
 ***********************************************************************/
#include "edge.h"

/***********************************************************************
 *  Method: edge::run
 *  Params: bool in
 * Returns: bool
 * Effects: 
 ***********************************************************************/

bool edge::run(bool in) {
    switch( whichEdge) {
        case RISING:
            output=positive(in);
            break;
        case FALLING:
            output=negative(in);
            break;
    }
    return output;
}


/***********************************************************************
 *  Method: edge::edge
 *  Params: TRANSITION t
 * Effects: 
 ***********************************************************************/
edge::edge(TRANSITION t) {
    whichEdge = t;
}

/***********************************************************************
 *  Method: edge::positive
 *  Params: bool in
 * Effects: 
 ***********************************************************************/
bool edge::positive(bool in) {

    if( (in == false) && (flag == false) ) {
        return false;
    }

    if( (in == false) && (flag == true) ) {
        flag = false;
        return false;
    }

    if( (in == true) && (flag == false )) {
        flag = true;
        return true;
    }

    if( (in == true) && (flag == true) ) {
        return false;
    }
}


/***********************************************************************
 *  Method: edge::negative
 *  Params: bool in
 * Effects: 
 ***********************************************************************/
bool edge::negative(bool in) {
    if( (in == false) && (flag == false) ) {
        flag = in;
        return false;
    }

    if( (in == false) && (flag == true) ) {
        flag = in;
        return true;
    }

    if( (in == true) && (flag == false )) {
        flag = in;
        return false;
    }

    if( (in == true) && (flag == true) ) {
        flag = in;
        return false;
    }
}


