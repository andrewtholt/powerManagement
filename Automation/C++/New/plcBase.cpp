#include <string.h>
#include <time.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <map>
#include <fstream>
#include <string>

#include "plcBase.h"

inline std::string &ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline std::string &rtrim(std::string& s, const char* t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

inline std::string &trim(std::string& s, const char* t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t);
}

using namespace std;

int plcBase::checkTime(string time ) {
    int retMinutes = -1;
    int hrs;
    int mins;
    string tmp="";
    int rc=0;
    
    char first = time.at(0);
    
    if ( first >= '0' && first <= '9' ) {
        rc=sscanf( time.c_str(),"%d:%d", &hrs, &mins);
        
        retMinutes = (hrs*60) + mins;
    } else {
        tmp = getValue( time );
        
        if( tmp != "") {
            rc=sscanf( tmp.c_str(),"%d:%d", &hrs, &mins);
            
            if(rc ==2 ) {
                retMinutes = (hrs*60) + mins;
            }
        }
    }
    
    return retMinutes;
}

int plcBase::getTimeMinutes() {
    int mins = 0;
    
    time_t rawtime;
    struct tm *timeinfo;
    
    time( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    mins = (timeinfo->tm_hour * 60 ) + timeinfo->tm_min ;
    
    return mins;
}

string plcBase::timeFromInt(int n) {
    char time[6];
    
    int hh = n / 60;
    int mm = n % 60;
    
    sprintf( time, "%02d:%02d", hh, mm );
    
    string t = time;
    
    return t;
    
}
string plcBase::addMinutes(string time, int adj) {
    int t = checkTime( time );
    
    t += adj;
    
    return (timeFromInt(t));
}

string plcBase::getTime() {
    //    cout << "plcBase::getTime" << endl;
    string res;
    
    time_t rawtime;
    struct tm *timeinfo;
    
    time( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    string hours   = to_string(timeinfo->tm_hour);
    string minutes = to_string(timeinfo->tm_min);
    
    res = hours +":" + minutes;
    
    return res;
    
}

bool plcBase::runNow(string time) {
    
    bool flag = false;
    
    int mins = checkTime( time );
    
    if( mins > 0) {
        int now = getTimeMinutes() ;
        
        flag = (now == mins) ;
    }
    
    return flag;
}
// TODO
// Modify all time based functions to use checkTime
//
bool plcBase::timeBetween(string start, string end) {
    
    int nowMinutes = getTimeMinutes();
    
    int startMinutes = checkTime( start );
    
    int endMinutes = checkTime(end);
    
    bool flag=false;
    
    if( (startMinutes >= 0 ) && (endMinutes >= 0)) {
        flag = (( startMinutes <= nowMinutes ) && ( nowMinutes < endMinutes ));
    }
    
    return flag;
}

void plcBase::plcDump() {
    printf("Hostname  : %s\n", hostName.c_str());
    printf("Port      : %d\n" , port);
}

void plcBase::setVerbose(bool flag) {
    verbose=flag;
}

bool plcBase::initPlc(string clientId) {
    printf("plcBase %s\n", clientId.c_str());
    return false;
}

plcBase::plcBase() {
    //    iam = name;
    hostName="UNKNOWN";
    port=0;
    initPlc("DEFAULT");
}

// You can set this only once.
//
bool plcBase::setHost(string host) {
    bool failFlag = true ;
    if( "UNKNOWN" == hostName) {
        hostName = host;
        failFlag = false;
    } else {
        failFlag = true;
    }
    
    return failFlag;
}
//
// You can set this only once.
//
bool plcBase::setPort(int p) {
    bool failFlag = true ;
    if( 0 == port ) {
        port = p;
        failFlag = false;
    } else {
        failFlag = true;
    }
    return failFlag;
}

void plcBase::emptyStack() {
    while(!logicStack.empty()) {
        logicStack.pop();
    }
}
bool plcBase::plcEnd(int ms) {
    bool failFlag=false;
    
    if(verbose) {
        cout << "plcBase::plcEnd" << endl;
    }
    
    emptyStack();

    if( ms > 0) {
        usleep(ms * 1000);
    }
    
    return failFlag;
}

const string plcBase::boolToString(bool f) {
    string state;
    
    state = (f) ? "ON" : "OFF" ;
    
    return state;
}

bool plcBase::stringToBool(string value) {
    bool state;
    
    if ( value == "YES" || value == "TRUE" || value == "ON" ) {
        state = true;
    }
    
    if ( value == "NO" || value == "FALSE" || value == "OFF" ) {
        state = false;
    }
    
    return state;
}


string plcBase::getValue( string symbol ) {
    bool v = ioPoint[ symbol ];
    // 
    // TODO database select value from iopoints where name = '<symbol'>
    // 
    string value = boolToString ( v );
    
    return value;
}

bool plcBase::getBoolValue( string symbol ) {
    return ioPoint[ symbol ];
}
bool plcBase::getBoolOldValue( string symbol ) {
    // TODO Call getValue
    return false;
}

bool plcBase::setValue( string symbol, string v ) {
    // TODO database
    // update ......
    ioPoint[ symbol ] = stringToBool(v);
    return false;
}
bool plcBase::setBoolValue( string symbol, bool v ) {
    // TODO database
    ioPoint[ symbol ] = v;
    return false;
}

void plcBase::Ld(string symbol) {
    if(verbose) {
        cout << "plcBase::Ld " << symbol ;
    }
    
    bool v=getBoolValue( symbol );
    
    logicStack.push( v );
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Ldn(string symbol) {
    if(verbose) {
        cout << "plcBase::Ldn " << symbol ;
    }
    
    bool v=getBoolValue( symbol );
    
    logicStack.push( !v );
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Ldr(string symbol) {
    cout << "plcBase::Ldr " << symbol ;
    
    static bool oldV = false;
    bool outV=false;
    
    bool v=getBoolValue( symbol );
    
    // So if oldV is low and new v is high then +ve edge
    outV =  !oldV && v;
    
    oldV = v;
    logicStack.push( outV );
    
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Ldf(string symbol) {
    cout << "plcBase::Ldf " << symbol ;
    
    static bool oldV = false;
    bool outV=false;
    
    bool v=getBoolValue( symbol );
    
    // So if oldV is high and new v is lo then -ve edge
    outV =  oldV && !v;
    
    oldV = v;
    logicStack.push( outV );
    
    cout << "   TOS: " << logicStack.top() << endl;
}
// 
// return the value TOS and
// remove it from the stack.
// 
bool plcBase::fromStack() {
    bool a = logicStack.top();
    logicStack.pop();
    
    return a;
}

void plcBase::toStack(bool v) {
    logicStack.push(v);
}
// 
// Return TOS, without removing it
//
bool plcBase::getTOS() {
    bool a = logicStack.top();
    
    return a;
}

void plcBase::setTOS(bool a) {
    logicStack.top();
    logicStack.push( a );
}

int plcBase::stackSize() {
    return logicStack.size();
}


void plcBase::Or(string symbol) {
    if(verbose) {
        cout << "plcBase::Or " << symbol ;
    }
    bool a;
    
    bool v = getBoolValue( symbol );
    
    a = fromStack() || v ;
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}


void plcBase::Orn(string symbol) {
    if(verbose) {
        cout << "plcBase::Orn " << symbol ;
    }
    
    bool v = getBoolValue( symbol );
    bool a;
    
    a = fromStack() || (!v) ;
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Orr(string symbol) {
    if(verbose) {
        cout << "plcBase::Orr " << symbol ;
    }
    
    bool a;
    bool outV=false;
    
    bool v = getBoolValue( symbol );
    bool oldV = getBoolOldValue( symbol );
    
    outV = v && !oldV;
    oldV = v;
    
    a = fromStack() ;
    a = a || outV;
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Orf(string symbol) {
    if(verbose) {
        cout << "plcBase::Orf " << symbol ;
    }
    
    static bool oldV = getBoolValue( symbol );
    bool outV=false;
    
    bool v = getBoolValue( symbol );
    
    bool a = fromStack() ;
    
    outV = !v && oldV;
    oldV = v;
    
    a = a || outV;
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::And(string symbol) {
    if(verbose) {
        cout << "plcBase::And " << symbol ;
    }
    
    bool v = getBoolValue( symbol );
    bool a = fromStack() ;
    
    a = a && v ;
    
    logicStack.push(a);
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Andn(string symbol) {
    if(verbose) {
        cout << "plcBase::Andn " << symbol ;
    }
    
    bool a = fromStack() ;
    bool v = getBoolValue( symbol );
    
    a = a && (!v);
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Andr(string symbol) {
    if(verbose) {
        cout << "plcBase::Andr " << symbol ;
    }
    
    bool outV=false;
    bool a = fromStack() ;
    
    bool v = getBoolValue( symbol );
    bool oldV = getBoolOldValue( symbol );
    
    outV = v && !oldV;
    oldV = v;
    
    a = a && outV ;
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::Andf(string symbol) {
    if(verbose) {
        cout << "plcBase::Andf " << symbol ;
    }
    
    bool outV=false;
    bool a = fromStack() ;
    
    bool v = getBoolValue( symbol );
    bool oldV = getBoolOldValue( symbol );
    
    outV = !v && oldV;
    oldV = v;
    
    a = a && outV ;
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}
// 
//
void plcBase::TimLd(string runAt) {
    if(verbose) {
        cout << "plcBase::TimLd " << runAt ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = runNow(runAt);
    
    logicStack.push( runFlag );
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimLd(string start, string end) {
    if(verbose) {
        cout << "plcBase::TimLd " << start << "-" << end ;
    }
    
    bool runFlag = false;
    
    runFlag = timeBetween( start, end ) ;
    logicStack.push( runFlag );
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimLdn(string runAt) {
    if(verbose) {
        cout << "plcBase::TimLdn " << runAt ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = runNow(runAt);
    
    logicStack.push( !runFlag );
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimLdn(string start, string end) {
    if(verbose) {
        cout << "plcBase::TimLdn " << start << "-" << end ;
    }
    bool runFlag = false;
    
    runFlag = !timeBetween( start, end ) ;
    logicStack.push( !runFlag );
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimAnd(string runAt) {
    if(verbose) {
        cout << "plcBase::TimAnd " << runAt ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = runNow(runAt);
    
    a = logicStack.top() && runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimAnd(string start, string end) {
    if(verbose) {
        cout << "plcBase::TimAnd " << start << "-" << end ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = timeBetween( start, end ) ;
    
    a = logicStack.top() && runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimAndn(string runAt) {
    if(verbose) {
        cout << "plcBase::TimAndn " << runAt ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = runNow(runAt);
    
    a = logicStack.top() && !runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimAndn(string start, string end) {
    if(verbose) {
        cout << "plcBase::TimAndn " << start << "-" << end ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = timeBetween( start, end ) ;
    
    a = logicStack.top() && !runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimOr(string runAt) {
    if(verbose) {
        cout << "plcBase::TimOr " << runAt;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = runNow(runAt);
    
    a = logicStack.top() || runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimOr(string start, string end) {
    if(verbose) {
        cout << "plcBase::TimOr " << start << "-" << end ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = timeBetween( start, end ) ;
    
    a = logicStack.top() || runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimOrn(string runAt) {
    if(verbose) {
        cout << "plcBase::TimOrn " << runAt;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = runNow(runAt);
    
    a = logicStack.top() || !runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcBase::TimOrn(string start, string end) {
    if(verbose) {
        cout << "plcBase::TimOrn " << start << "-" << end ;
    }
    
    bool runFlag=false;
    bool a = false;
    
    runFlag = timeBetween( start, end ) ;
    
    a = logicStack.top() || !runFlag;
    logicStack.pop();
    
    logicStack.push(a);
    
    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}
// 
// TODO These need some thought
// Have an update output function that is overridden by children
//
void plcBase::Outn(string symbol) {
    if(verbose) {
        cout << "plcBase::Outn " << symbol ;
    }
    
    bool a = fromStack() ;
    
    string accString = symbol + " ";
    
    accString += (a) ? "FALSE" : "TRUE";
    
    //    ioPoint[ symbol ] = a;
    
    while( !logicStack.empty() ) {
        logicStack.pop();
    }
    //    acc = false;
}

void plcBase::Out(string symbol) {
    
    bool a = fromStack() ;
    
    string accString = symbol + " ";
    
    accString += (a) ? "TRUE" : "FALSE";
    
    // TODO
    // Replace with SQL update.
    //
    //    ioPoint[ symbol ] = a;
    
    // TODO
    // Replace with:
    // update sql databas
    // and publish.
    //    int rc =  SPTxSimple((char *)outGroup.c_str(), (char *)accString.c_str()) ;    
    
    while( !logicStack.empty() ) {
        logicStack.pop();
    }
    //    acc = false;
}
