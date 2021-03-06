
#pragma once

#include <sqlite3.h>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <hspread.h>

using namespace std;


class plc {
    private:
//        bool acc=false;

        stack<bool> logicStack;

        map<string, bool> ioPoint;
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
        bool runNow(string when);
        
        bool fromStack();
        void Ld(string symbol);
        void Ldn(string symbol);
        void Ldr(string symbol);
        void Ldf(string symbol);

        void Or(string symbol);
        void Orn(string symbol);
        void Orr(string symbol);
        void Orf(string symbol);

        void And(string symbol);
        void Andn(string symbol);
        void Andr(string symbol);
        void Andf(string symbol);

        void TimLd(string time);    // what was the symbol for an IO point
                                    // is now hh:mm time.
        void TimAndn(string time);

        void Out(string symbol);
        void Outn(string symbol);

        const string boolToString(bool f);
        void instDisplay(string inst, string iop);
    public:
        void setVerbose(bool flag);
        void dumpProgram();
        plc();
        plc(string name);
        plc(string name, string progFile);

        bool loadProg(string fileName);

        void plcDump();
        bool plcStatus();
        void plcRun();
};
