
#pragma once

#include <sqlite3.h>
#include <string>
#include <map>
#include <vector>
#include <stack>
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

    TIM_LD=0x30,
    TIM_LDN,
    TIM_AND,
    TIM_ANDN,

    OUT=0x40,
    OUTN,
    END=0xff
};


class plcBase {
    private:

        stack<bool> logicStack;

        string iam;

        bool verbose=false;

        void initPlc();
        
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
        plcBase();
};
