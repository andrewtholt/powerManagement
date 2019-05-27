/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//plcBase.cpp
 *   DATE: Thu Mar  7 18:19:45 2019
 *  DESCR: 
 ***********************************************************************/
#include "plcBase.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <cstring>
#include <string>

/*
#include <nlohmann/json.hpp>

using json = nlohmann::json;
*/

using namespace std;

void plcBase::display() {
    bool flag = logicStack.top( );

    if( flag ) {
        cout << "\tON" << endl;
    } else {
        cout << "\tOFF" << endl;
    }

}
void plcBase::setVerbose(bool flag) {
	verbose = flag;
}
/***********************************************************************
 *  Method: plcBase::getValue
 *  Params: string shortName
 * Returns: string
 * Effects: 
 ***********************************************************************/
string plcBase::getValue(string shortName) {
//    json rec = getRow(shortName);

//    return(rec["msg"]);
    return("UNIMPLEMENTED");
}


/***********************************************************************
 *  Method: plcBase::setValue
 *  Params: string shortName, string v
 * Returns: bool
 * Effects: 
 ***********************************************************************/
void plcBase::setValue(string shortName, string v) {
    /*
    json outMsg=getRow( shortName) ;

    outMsg["msgType"]  = "DATA";
    outMsg["cmd"]      = "SET";
    outMsg["shortName"]=shortName;
    outMsg["direction"]="OUT";
    outMsg["msg"]=v;

    string out = outMsg.dump();
    write(fd, out.c_str(), out.length());
    */
}

/***********************************************************************
 *  Method: plcBase::getOldValue
 *  Params: string
 * Returns: string
 * Effects: 
 ***********************************************************************/
string plcBase::getOldValue(string) {
    string ov="";

    return ov;
}


/***********************************************************************
 *  Method: plcBase::setOldValue
 *  Params: string shortName, string v
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool plcBase::setOldValue(string shortName, string v) {
    bool failFlag=true;

    return failFlag;
}


/***********************************************************************
 *  Method: plcBase::getBoolValue
 *  Params: string
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool plcBase::getBoolValue(string name) {
    bool value=false;

    string v = getValue(name);

    if( v == "UNKNOWN") {
        v= false;
    }

    if( v == "UNDEFINED") {
        v= false;
    }

    if( v == "NODATA") {
        v= false;
    }

    value = (v=="ON") ? true : false ;

    return value;
}


/***********************************************************************
 *  Method: plcBase::setBoolValue
 *  Params: string shortName, bool v
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool plcBase::setBoolValue(string shortName, bool v) {
    bool failFlag=true;

    return failFlag;
}


/***********************************************************************
 *  Method: plcBase::getBoolOldValue
 *  Params: string
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool plcBase::getBoolOldValue(string) {
    bool failFlag=true;

    return failFlag;
}


/***********************************************************************
 *  Method: plcBase::setBoolOldValue
 *  Params: string shortName, bool v
 * Returns: bool
 * Effects: 
 ***********************************************************************/

bool plcBase::setBoolOldValue(string shortName, bool v) {
    bool failFlag=true;

    return failFlag;
}


/***********************************************************************
 *  Method: plcBase::plcBase
 *  Params: 
 * Effects: 
 ***********************************************************************/
plcBase::plcBase() {
    cout << "PLC Base" << endl;
}
//
// return the value TOS and
// remove it from the stack.
//
bool plcBase::Pop() {
    bool a = logicStack.top();
    logicStack.pop();

    return a;
}

void plcBase::Push(bool flag) {
    logicStack.push( flag );
}

void plcBase::Ld(string symbol) {
    try {
        cacheHit = true;
        if(verbose) {
            cout << "plcBase::Ld " << symbol ;
        }

        bool v=getBoolValue( symbol );
        logicStack.push( v );

        if(verbose) {
            cout << "   TOS: " << logicStack.top() << endl;
        }
    } catch(int e) {
        cerr << symbol + ":Undefined " << e << endl;
    }
}

void plcBase::And() {
    if(verbose) {
        cout << "plcBase::And " ;
    }

    bool a = Pop()  ;
    bool b = Pop()  ;

    bool v = a&b;
    logicStack.push(v);

    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Or() {
    if(verbose) {
        cout << "plcBase::Or " ;
    }

    bool a = Pop()  ;
    bool b = Pop()  ;

    bool v = a|b;
    logicStack.push(v);

    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Invert() {
    if(verbose) {
        cout << "plcBase::Not " ;
    }

    bool a = Pop()  ;

    bool v = !a;
    logicStack.push(v);

    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Out(string symbol) {
// Overload
}

/***********************************************************************
 *  Method: plcBase::End
 *  Params: int delay (in mS)
 * Returns: void
 * Effects: 
 ***********************************************************************/
void plcBase::End(int delay) {
    while ( ! logicStack.empty() ) {
        logicStack.pop();
    }

    // 
    // Simple, but niave solution.
    // 
    // First time through wait delay ms then get time
    // second time
    // 
    usleep( delay * 1000);
}

#ifdef __cplusplus
extern "C" {
    /*
       struct plcBase *newPlcBase(spreadBase *spr) {
       struct plcBase *ptr = new plcBase(spr);

       return ptr ;
       }
     */

#endif

#ifdef __cplusplus
}
#endif
