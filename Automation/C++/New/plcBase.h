
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
        map<string, bool> ioPoint;

    public:
        virtual string getTime(); // get time in hh:mm:ss format
        virtual bool runNow(string ); // pass in time as hh:mm, return true if that time is now.
        virtual bool timeBetween(string, string );  // pass in time as hh:mm, 
                                                    // return true if that time is between the two.

        virtual string getValue( string );
        virtual bool getBoolValue( string );
        virtual bool setBoolValue(string shortName, bool v );
        virtual bool setValue(string shortName, string v );

        virtual void Ld(string symbol);
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

        void TimLd(string time);     // True if time is now
        void TimLd(string, string ); // True if now is between 1sy & second time    

        void TimLdn(string time);     // False if time is now
        void TimLdn(string, string ); // False if now is between 1sy & second time    

        void TimAnd(string time);
        void TimAnd(string start, string end);

        void TimAndn(string time);
        void TimAndn(string start, string end);

        void TimOr(string time);
        void TimOr(string start, string end);

        void Out(string symbol);
        void Outn(string symbol);

        void TimOrn(string time);
        void TimOrn(string start, string end);

        const string boolToString(bool f);
        bool stringToBool(string);
        void instDisplay(string inst, string iop);
        void setVerbose(bool flag);
        void plcDump();
        plcBase();
        bool setHost(string);
        bool setPort(int);
};
