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

    int mins = checkTime( time );
    string now=getTime();
    bool flag = (now == time) ;

    return flag;
}

bool plcBase::timeBetween(string start, string end) {

    int nowHH = 0;
    int nowMM = 0;

    int startHH = 0;
    int startMM = 0;

    int endHH = 0;
    int endMM = 0;

    string now=getTime();

    sscanf( now.c_str(),"%d:%d", &nowHH, &nowMM);
    int nowMinutes= (nowHH * 60) + nowMM;

    sscanf( start.c_str(),"%d:%d", &startHH, &startMM);
    int startMinutes = (startHH * 60) + startMM;

    sscanf( end.c_str(),"%d:%d", &endHH, &endMM);
    int endMinutes = (endHH * 60) + endMM;

    bool flag = (( startMinutes <= nowMinutes ) && ( nowMinutes < endMinutes ));

    return flag;
}

void plcBase::plcDump() {
    printf("Hostname  : %s\n", hostName.c_str());
    printf("Port      : %d\n" , port);
}

void plcBase::setVerbose(bool flag) {
    verbose=flag;
}

bool plcBase::initPlc() {
    printf("plcBase\n");
    return false;
}

plcBase::plcBase() {
//    iam = name;
    hostName="UNKNOWN";
    port=0;
    initPlc();
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
/*
plcBase::plcBase(string name, string host) {
//    iam = name;

    setServer( (char *)host.c_str() );
    initPlc();
}

bool plcBase::plcStatus() {
    return spreadOK;
}


bool plcBase::loadProg(string fileName) {
    bool failFlag=true;
    string line;
    string inLine;
    string token;
    size_t pos;

    string inst;
    string iop;
    string tmp;

    ifstream inFile( fileName);

    while(getline(inFile, inLine )) {
        // Find and strip comments.
        //
        pos = inLine.find(";");
        if( pos != string::npos)  {
            if ( pos >= 0) {
                line = inLine.substr(0,pos);
            }
        } else {
            line=inLine;
        }
        if( line.size() > 0) {
            //
            // line now contains just the stuff I want.
            // The line format is <inst> <io point>
            //
            pos = line.find(" ");
            if( pos != string::npos)  {
                inst = line.substr(0, pos);
                if (pos >= 0) {
                    iop = line.substr(pos+1);
                }
                inst = trim(inst);
                iop  = trim(iop);

//                for (auto & c: iop) c = toupper(c);

                compile(inst,iop);
            }
        }
    }

    return failFlag;
}

void plcBase::dumpProgram() {
    for (auto i : RAM ) {
        printf("0x%02x %s\n", i.inst, (char *)i.iop.c_str());
    }
}

void plcBase::compile(string inst, string iop) {
    string tmp = inst.substr(0,2);

    struct ramEntry fred;

    cout << inst + " " + iop << endl;

    if( tmp == "LD" ) {
        if ( inst[2] == 'N' ) {
            fred.inst = LDN;
        } else if ( inst[2] == 'R' ) {
            fred.inst = LDR;
        } else if ( inst[2] == 'F' ) {
            fred.inst = LDF;
        } else {
            fred.inst = LD;
        }
    } else if( tmp == "OR") {
        if ( inst[2] == 'N' ) {
            fred.inst = ORN;
        } else if ( inst[2] == 'R' ) {
            fred.inst = ORR;
        } else if ( inst[2] == 'F' ) {
            fred.inst = ORF;
        } else {
            fred.inst = OR;
        }
    } else if( tmp == "AN") {
        if ( inst[3] == 'N' ) {
            fred.inst = ANDN;
        } else if ( inst[3] == 'R' ) {
            fred.inst = ANDR;
        } else if ( inst[3] == 'F' ) {
            fred.inst = ANDF;
        } else {
            fred.inst = AND;
        }
    } else if( tmp == "OU") {
        if(inst == "OUT" ) {
            fred.inst = OUT;
        } else if(inst == "OUTN" ) {
            fred.inst = OUTN;
        }
    } else if( tmp == "TI") {
        if(inst == "TIM-LD") {
            fred.inst = TIM_LD;
        } else if(inst == "TIM-ANDN") {
            fred.inst = TIM_ANDN;
        }
    }
    if( iop.size() > 0 ) {
        fred.iop = iop;
    } else {
        fred.iop ="";
    }
    RAM.push_back(fred);
}
*/

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

    string value = boolToString ( v );

    return value;
}

bool plcBase::getBoolValue( string symbol ) {
    return ioPoint[ symbol ];
}

bool plcBase::setValue( string symbol, string v ) {
    ioPoint[ symbol ] = stringToBool(v);
    return false;
}
bool plcBase::setBoolValue( string symbol, bool v ) {
    ioPoint[ symbol ] = v;
    return false;
}

void plcBase::Ld(string symbol) {
    cout << "plcBase::Ld " << symbol ;

    bool v=getBoolValue( symbol );

    logicStack.push( v );
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Ldn(string symbol) {
    bool v=getBoolValue( symbol );

    logicStack.push( !v );

    cout << "   TOS: " << logicStack.top() << endl;
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

void plcBase::Or(string symbol) {
    cout << "plcBase::Or " << symbol ;
    bool a;

    bool v = getBoolValue( symbol );

    a = fromStack() || v ;
    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}


void plcBase::Orn(string symbol) {
    cout << "plcBase::Orn " << symbol ;

    bool v = getBoolValue( symbol );
    bool a;

    a = fromStack() || !v ;
    logicStack.push(a);

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Orr(string symbol) {
    cout << "plcBase::Orr " << symbol ;
    static bool oldV = getBoolValue( symbol );
    bool outV=false;
    bool a;

    bool v = getBoolValue( symbol );

    outV = v && !oldV;
    oldV = v;

    a = fromStack() ;
    a = a || outV;
    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Orf(string symbol) {
    cout << "plcBase::Orf " << symbol ;
    static bool oldV = getBoolValue( symbol );
    bool outV=false;

    bool v = getBoolValue( symbol );

    bool a = fromStack() ;

    outV = !v && oldV;
    oldV = v;

    a = a || outV;
    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::And(string symbol) {
    cout << "plcBase::And " << symbol ;

    bool v = getBoolValue( symbol );
    bool a = fromStack() ;

    a = a && v ;

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Andn(string symbol) {
    cout << "plcBase::Andn " << symbol ;

    bool a = fromStack() ;
    bool v = getBoolValue( symbol );

    a = a && (!v);
    logicStack.push(a);

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Andr(string symbol) {
    cout << "plcBase::Andr " << symbol ;

    static bool oldV=false;
    bool outV=false;
    bool a = fromStack() ;

    bool v = getBoolValue( symbol );

    outV = v && !oldV;
    oldV = v;

    a = a && outV ;
    logicStack.push(a);

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::Andf(string symbol) {
    cout << "plcBase::Andf " << symbol ;

    static bool oldV=false;
    bool outV=false;
    bool a = fromStack() ;

    bool v = getBoolValue( symbol );

    outV = !v && oldV;
    oldV = v;

    a = a && outV ;
    logicStack.push(a);

    cout << "   TOS: " << logicStack.top() << endl;
}
// 
//
void plcBase::TimLd(string runAt) {
    cout << "plcBase::TimLd " << runAt ;
    bool runFlag=false;
    bool a = false;

    runFlag = runNow(runAt);

    logicStack.push( runFlag );

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimLd(string start, string end) {
    cout << "plcBase::TimLd " << start << "-" << end ;
    bool runFlag = false;

    runFlag = timeBetween( start, end ) ;
    logicStack.push( runFlag );

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimLdn(string runAt) {
    cout << "plcBase::TimLdn " << runAt ;
    bool runFlag=false;
    bool a = false;

    runFlag = runNow(runAt);

    logicStack.push( !runFlag );

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimLdn(string start, string end) {
    cout << "plcBase::TimLdn " << start << "-" << end ;
    bool runFlag = false;

    runFlag = timeBetween( start, end ) ;
    logicStack.push( !runFlag );

    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimAnd(string runAt) {
    cout << "plcBase::TimAnd " << runAt ;

    bool runFlag=false;
    bool a = false;

    runFlag = runNow(runAt);

    a = logicStack.top() && runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimAnd(string start, string end) {
    cout << "plcBase::TimAnd " << start << "-" << end ;

    bool runFlag=false;
    bool a = false;

    runFlag = timeBetween( start, end ) ;

    a = logicStack.top() && runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimAndn(string runAt) {
    cout << "plcBase::TimAndn " << runAt ;

    bool runFlag=false;
    bool a = false;

    runFlag = runNow(runAt);

    a = logicStack.top() && !runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimAndn(string start, string end) {
    cout << "plcBase::TimAndn " << start << "-" << end ;

    bool runFlag=false;
    bool a = false;

    runFlag = timeBetween( start, end ) ;

    a = logicStack.top() && !runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimOr(string runAt) {
    cout << "plcBase::TimOr " << runAt;

    bool runFlag=false;
    bool a = false;

    runFlag = runNow(runAt);

    a = logicStack.top() || runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimOr(string start, string end) {
    cout << "plcBase::TimOr " << start << "-" << end ;

    bool runFlag=false;
    bool a = false;

    runFlag = timeBetween( start, end ) ;

    a = logicStack.top() || !runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimOrn(string runAt) {
    cout << "plcBase::TimOrn " << runAt;

    bool runFlag=false;
    bool a = false;

    runFlag = runNow(runAt);

    a = logicStack.top() || !runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}

void plcBase::TimOrn(string start, string end) {
    cout << "plcBase::TimOrn " << start << "-" << end ;

    bool runFlag=false;
    bool a = false;

    runFlag = timeBetween( start, end ) ;

    a = logicStack.top() || runFlag;
    logicStack.pop();

    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}
// 
// TODO These need some thought
// Have an update output function that is overridden by children
//
void plcBase::Outn(string symbol) {
    cout << "plcBase::Outn " << symbol ;

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
