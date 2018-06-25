#include "myClientClass.h"
#include <string.h>
#include "plc.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <fstream>
#include <string>

inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t);
}

using namespace std;

void plc::plcDump() {
    cout << "I am      :" + iam << endl;
    cout << "PLC Status:";
    
    if( spreadOK) {
        cout << "OK" << endl;
    } else {
        cout << "Failed" << endl;
    }
    
    cout << "In  group :" + inGroup << endl;
    cout << "Out group :" + outGroup << endl;
    
    cout << endl;
    
}

void plc::setVerbose(bool flag) {
    verbose=flag;
}

void plc::initPlc() {
    
    ioPoint["FALSE"] = false;
    ioPoint["TRUE"] = true;
    ioPoint["SCAN"] = false;
    
    setUser((char *)iam.c_str());
    
    int rc=SPConnectSimple();
    
    spreadOK = ( rc < 0 ) ? false : true ;
    
    if( spreadOK ) {
        rc=SPJoinSimple((char *)"global");
        spreadOK = ( rc < 0 ) ? false : true ;
        
        rc=SPJoinSimple((char *)"logic");
        spreadOK = ( rc < 0 ) ? false : true ;
    }
    
}

plc::plc(string name) {
    iam = name;
    
    initPlc();
}

plc::plc(string name, string host) {
    iam = name;
    
    setServer( (char *)host.c_str() );
    initPlc();
}

bool plc::plcStatus() {
    return spreadOK;
}

bool plc::loadProg(string fileName) {
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
            //            cout << "LINE:" + line << endl;
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
                //                cout << "\t>" + inst +"<" << endl;
                //                cout << "\t:" + iop + "<" << endl;
                
                
                compile(inst,iop);
            }
        }
    }
    
    return failFlag;
}

void plc::dumpProgram() {
    for (auto i : RAM ) {
        printf("0x%02x %s\n", i.inst, (char *)i.iop.c_str());
    }
}
void plc::compile(string inst, string iop) {
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
    }
    if( iop.size() > 0 ) {
        fred.iop = iop;
    } else {
        fred.iop ="";
    }
    RAM.push_back(fred);
}

void plc::plcRun() {
    
    char rxMsg[255];
    char *tmpI;
    char *tmpV;
    bool boolV=false;
    
    while(true) {
        int rc = SPRxSimple(rxMsg, 255);
        
        tmpI = strtok(rxMsg," ");
        tmpV = strtok(NULL, " \n");
        
        string thing = tmpI;
        thing = trim(thing);
        
        
        if( thing == "TICK") {
            cout << thing << endl;
        } else {
            boolV = (!strcmp(tmpV,"TRUE")) ? true : false; 
            
            ioPoint[ thing ] = boolV;
        }
        
        for (auto i : RAM ) {
            //        printf("0x%02x %s\n", i.inst, (char *)i.iop.c_str());
            switch(i.inst) {
                case LD: 
                    if( verbose) {
                        printf("LD\t%s\n", (char *)i.iop.c_str());
                    }
                    Ld(i.iop);
                    break;
                case OR:
                    if( verbose) {
                        printf("OR\t%s\n", (char *)i.iop.c_str());
                    }
                    Or(i.iop);
                    break;
                case ANDN:
                    if( verbose) {
                        printf("ANDN\t%s\n", (char *)i.iop.c_str());
                    }
                    Andn(i.iop);
                    break;
                case OUT:
                    if( verbose) {
                        printf("OUT\t%s\n", (char *)i.iop.c_str());
                    }
                    Out(i.iop);
                    break;
            }
        }
    }
}

void plc::Ld(string symbol) {
    bool v=ioPoint[ symbol];
    
    acc=v;
}

// TODO Test
void plc::Ld(string symbol) {
    bool v=ioPoint[ symbol];
    
    acc=!v;
}

// TODO Test
void plc::Ldr(string symbol) {
    static bool oldV = false;
    bool outV=false;

    bool v=ioPoint[ symbol];

// So if oldV is low and new v is high then +ve edge
    outV =  !oldV && v;

    oldV = v;
    acc=outV;
}

// TODO Test
void plc::Ldf(string symbol) {
    static bool oldV = false;
    bool outV=false;

    bool v=ioPoint[ symbol];

// So if oldV is high and new v is lo then -ve edge
    outV =  oldV && !v;

    oldV = v;
    acc=outV;
}

void plc::Or(string symbol) {
    bool v=ioPoint[ symbol];
    
    acc = acc || v;
}

void plc::Andn(string symbol) {
    bool v=ioPoint[ symbol];
    
    acc = acc && (!v);
}

void plc::Out(string symbol) {
    
    string accString;
    
    accString = symbol + " ";
    
    accString += (acc) ? "TRUE" : "FALSE";
    
    ioPoint[ symbol ] = acc;
    
    int rc =  SPTxSimple((char *)outGroup.c_str(), (char *)accString.c_str()) ;    
    
    acc = false;
}
