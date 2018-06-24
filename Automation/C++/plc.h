
#pragma once

#include <sqlite3.h>
#include <string>
#include <map>
#include <vector>
#include <hspread.h>

using namespace std;

enum Inst { 
    NOP=0, 
    LD, 
    LDN,
    LDR,
    LDF,

    OR=0x10,
    ORN,
    ORR,
    ORF,

    AND=0x20,
    ANDN,
    ANDR,
    ANDF,

    OUT=0x30,
    OUTN,
    END=0xff
};


class plc {
    private:
        struct ramEntry {
            uint8_t inst;
            string iop;
        } ;
        
        bool acc=false;

        map<string, bool> ioPoint;
//        vector<array<string,2>> RAM;
        vector<ramEntry> RAM;

        string iam;
        string spreadHost;
        string inGroup="logic";     // Joins this to be notified of inputs
        string outGroup="output";  // DOesn't join this, send results.

        bool spreadOK=false;
        int spreadError = 0;
        bool verbose=false;

        void initPlc();
        void compile(string inst, string iop);
        
        void Ld(string symbol);
        void Or(string symbol);
        void Andn(string symbol);
        void Out(string symbol);
    public:
        void setVerbose(bool flag);
        void dumpProgram();
        plc(string name);
        plc(string name, string progFile);

        bool loadProg(string fileName);

        void plcDump();
        bool plcStatus();
        void plcRun();
};
