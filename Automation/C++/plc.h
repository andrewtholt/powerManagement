
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

    OR,
    ORN,
    ORR,
    ORF,

    AND,
    ANDN,
    ANDR,
    ANDF,

    OUT,
    OUTN,
    END=0xff
};


class plc {
    private:
        struct ramEntry {
            uint8_t inst;
            string iop;
        } ;

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
        void dumpProgram();
    public:
        plc(string name);
        plc(string name, string progFile);

        void setVerbose(bool v);

        bool loadProg(string fileName);

        void plcDump();
        bool plcStatus();
        void plcRun();
};
