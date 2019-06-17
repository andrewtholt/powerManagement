/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//toggle.cpp
 *   DATE: Mon Jun 17 07:29:10 2019
 *  DESCR: 
 ***********************************************************************/
#include "toggle.h"

/***********************************************************************
 *  Method: toggle::run
 *  Params: bool in
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool toggle::run(bool in) {
    if ( in == false && flag == false) {
        output = false;
    }
    
    if( in == true && flag == false ) {
        output = !output;
        flag=true;
    }

    if( in == false && flag == true ) {
        flag=false;
    }

    return output;
}


