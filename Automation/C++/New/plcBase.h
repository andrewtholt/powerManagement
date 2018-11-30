
#pragma once

#include <sqlite3.h>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <hspread.h>

using namespace std;


class plcBase {
    protected:
        string hostName;
        int port;
        stack<bool> logicStack;
        bool fromStack();
        bool verbose=false;

    private:

        string iam;
        bool initPlc();
        bool connect(string, string);

    public:
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
        bool stringToBool(string);
        void instDisplay(string inst, string iop);
        void setVerbose(bool flag);
        void plcDump();
        plcBase();
        bool setHost(string);
        bool setPort(int);
};
