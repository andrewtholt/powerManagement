#include <string.h>
#include <time.h>
#include <stdio.h>
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

void plcBase::plcDump() {
    printf("Hostname  : %s\n", hostName.c_str());
    printf("Port      : %d\n" , port);
}

void plcBase::setVerbose(bool flag) {
    verbose=flag;
}

void plcBase::initPlc() {
    printf("plcBase\n");

}

plcBase::plcBase() {
//    iam = name;
    hostName="UNKNOWN";
    port=0;
    initPlc();
}

// You can set this only once.
//
void plcBase::setHost(string host) {
    if( "UNKNOWN" != hostName) {
        hostName = host;
    }
        
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

    state = (f) ? "True" : "False" ;

    return state;
}

/*
void plcBase::instDisplay(string inst, string iop) {

    cout << "Stack :";

    for (stack<bool> dump = logicStack; !dump.empty(); dump.pop()) {
        cout << dump.top() << ", ";
    }

    std::cout << "(" << logicStack.size() << " elements)\n";

    cout << "\t" + inst + "\t" + iop + "\t = " ;

    if ( ioPoint[iop] ) {
        cout << "[True]";
    } else {
        cout << "[False]";
    }

    cout << endl;
}

void plcBase::plcRun() {

    char rxMsg[255];
    char *tmpI;
    char *tmpV;
    bool boolV=false;
    bool firstPass=true;
    int rc;

    while(true) {
        if( firstPass ) {
            firstPass=false;
        } else {
            rc = SPRxSimple(rxMsg, 255);

            tmpI = strtok(rxMsg," ");
            tmpV = strtok(NULL, " \n");

            string thing = tmpI;
            thing = trim(thing);


            if( thing == "TICK") {
                if( verbose) {
                    cout << thing << endl;
                }
            } else {
                boolV = (!strcmp(tmpV,"TRUE")) ? true : false; 

                ioPoint[ thing ] = boolV;
            }
        }

        for (auto i : RAM ) {

            switch(i.inst) {
                case LD: 
                    if( verbose) {
                        instDisplay("LD", i.iop);
                    }
                    Ld(i.iop);
                    break;
                case LDN: 
                    if( verbose) {
                        instDisplay("LDN", i.iop);
                    }
                    Ldn(i.iop);
                    break;
                case LDR: 
                    if( verbose) {
                        instDisplay("LDR", i.iop);
                    }
                    Ldr(i.iop);
                    break;
                case LDF: 
                    if( verbose) {
                        instDisplay("LDF", i.iop);
                    }
                    Ldf(i.iop);
                    break;
                case OR:
                    if( verbose) {
                        instDisplay("OR", i.iop);
                    }
                    Or(i.iop);
                    break;
                case ORN:
                    if( verbose) {
                        instDisplay("ORN", i.iop);
                    }
                    Orn(i.iop);
                    break;
                case ORR:
                    if( verbose) {
                        printf("ORR\t%s\n", (char *)i.iop.c_str());
                    }
                    Orr(i.iop);
                    break;
                case ORF:
                    if( verbose) {
                        instDisplay("ORF", i.iop);
                    }
                    Orf(i.iop);
                    break;
                case AND:
                    if( verbose) {
                        instDisplay("AND", i.iop);
                    }
                    And(i.iop);
                    break;
                case ANDR:
                    if( verbose) {
                        instDisplay("ANDR", i.iop);
                    }
                    Andr(i.iop);
                    break;
                case ANDF:
                    if( verbose) {
                        instDisplay("ANDF", i.iop);
                    }
                    Andf(i.iop);
                    break;
                case ANDN:
                    if( verbose) {
                        instDisplay("ANDN", i.iop);
                    }
                    Andn(i.iop);
                    break;
                case OUT:
                    if( verbose) {
                        instDisplay("OUT", i.iop);
                    }
                    Out(i.iop);
                    break;
                case TIM_LD:
                    if(verbose) {
                        instDisplay("TIM-LD", i.iop);
                    }
                    TimLd(i.iop);
                    break;
                case TIM_ANDN:
                    if(verbose) {
                        instDisplay("TIM-ANDN", i.iop);
                    }
                    TimAndn(i.iop);
                    break;
                case OUTN:
                    if( verbose) {
                        instDisplay("OUTN", i.iop);
                    }
                    Outn(i.iop);
                    break;
            }
        }
    }
}
*/

void plcBase::Ld(string symbol) {
    // TODO
    // Replace with "select value from plc where shorthand = 'symbol'"
    // Followed by convert YES, TRUE, ON to true, and
    // NO, FALSE, OFF to false
    //
    bool v=false;

    logicStack.push( v );
//    acc=v;
}

void plcBase::Ldn(string symbol) {
//    bool v=ioPoint[ symbol];
    bool v=false;

    logicStack.push( !v );

//    acc=!v;
}

void plcBase::Ldr(string symbol) {
    static bool oldV = false;
    bool outV=false;

    string sqlCmd = "select value from plc where shortname = '" + symbol + "';";

    // run above and set return in v

    bool v=false ;

    // So if oldV is low and new v is high then +ve edge
    outV =  !oldV && v;

    oldV = v;
    logicStack.push( outV );
//    acc=outV;
}

void plcBase::Ldf(string symbol) {
    static bool oldV = false;
    bool outV=false;

//    bool v=ioPoint[ symbol];
    bool v = false;

    // So if oldV is high and new v is lo then -ve edge
    outV =  oldV && !v;

    oldV = v;
    logicStack.push( outV );
//    acc=outV;
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
//    bool v=ioPoint[ symbol];
    bool v = false;
    bool a;

    a = fromStack() || v ;
    logicStack.push(a);

//    acc = acc || v;
}

void plcBase::Orn(string symbol) {
//    bool v=ioPoint[ symbol];
    bool v = false;
    bool a;

    a = fromStack() || !v ;
    logicStack.push(a);

//    acc = acc || !v;
}

void plcBase::Orr(string symbol) {
    static bool oldV=false;
    bool outV=false;
    bool a;

//    bool v=ioPoint[ symbol];
    bool v=false;

    outV = v && !oldV;
    oldV = v;

    a = fromStack() ;
    a = a || outV;
    logicStack.push(a);
}

void plcBase::Orf(string symbol) {
    static bool oldV=false;
    bool outV=false;

//    bool v=ioPoint[ symbol];
    bool v=false;

    bool a = fromStack() ;

    outV = !v && oldV;
    oldV = v;

    a = a || outV;
    logicStack.push(a);
}

void plcBase::And(string symbol) {
//    bool v=ioPoint[ symbol];
    bool v = false;
    bool a = fromStack() ;

    a = a && v ;

    logicStack.push(a);
}

void plcBase::Andn(string symbol) {
//    bool v=ioPoint[ symbol];
    bool v = false;
    bool a = fromStack() ;

    a = a && (!v);
    logicStack.push(a);
}

void plcBase::Andr(string symbol) {
    static bool oldV=false;
    bool outV=false;
    bool a = fromStack() ;

//    bool v=ioPoint[ symbol];
    bool v = false;
    outV = v && !oldV;
    oldV = v;

    a = a && outV ;
    logicStack.push(a);
}

void plcBase::Andf(string symbol) {
    static bool oldV=false;
    bool outV=false;
    bool a = fromStack() ;

//    bool v=ioPoint[ symbol];
    bool v = false;

    outV = !v && oldV;
    oldV = v;

    a = a && outV ;
    logicStack.push(a);
}
/*
bool plcBase::runNow(string when) {
    bool runNow=false;

    char target[16];  // hh:mm-hh:mm
    
    char timeFrom[6];
    char timeTo[6];
    
    uint32_t fromMins=0;
    uint32_t toMins=0;
    
    strcpy(target, when.c_str());
    
    char *tmp = strtok(target, "-");
    strcpy(timeFrom, tmp);
    
    tmp = strtok(NULL, "-");
    strcpy(timeTo, tmp);
    // 
    // Convert time from into minutes since midnight.
    // 
    tmp = strtok(timeFrom,":");
    fromMins = atoi(tmp)*60;
    
    tmp = strtok(NULL, ":");
    fromMins += atoi(tmp);
    // 
    // Convert time TO into minutes since midnight.
    // 
    tmp = strtok(timeTo,":");
    toMins = atoi(tmp)*60;
    
    tmp = strtok(NULL, ":");
    toMins += atoi(tmp);
    
    time_t now=time(NULL);
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    
    int nowMins = (hours*60) + minutes;
    
    runNow = ( nowMins >= fromMins && nowMins <= toMins) ? true : false ;

//    strcpy(target, when.c_str());
//
//    char *hrs = strtok(target,(char *)":");
//    char *min = strtok(NULL,(char *)" :");
//
//    int minRun = atoi(min);

//    runNow = ( minRun == minutes) ? true:false;

    return runNow;
}
*/

void plcBase::TimLd(string runAt) {
    static bool hasRun=false;
    bool runFlag=false;
    bool a = false;

//    runFlag = runNow(runAt);

    if( runFlag && !hasRun ) {
        a = true;
        hasRun = true;
    }

    if ( !runFlag) {
        a = false;
        hasRun = false;
    }
    logicStack.push(a);
}

void plcBase::TimAndn(string runAt) {
    static bool hasRun=false;
    bool runFlag=false;
    bool a = false;

//    runFlag = runNow(runAt);

    if( runFlag && !hasRun ) {
        a = a && false;
        hasRun = true;
    }

    if ( !runFlag) {
        a = a && true ;
        hasRun = false;
    }
    logicStack.push(a);
}

void plcBase::Outn(string symbol) {

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
