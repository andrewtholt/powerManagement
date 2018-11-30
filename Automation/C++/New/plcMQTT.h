
#pragma once

/*
#include <sqlite3.h>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <hspread.h>
*/

#include <mosquitto.h>

#include "plcMQTT.h"
#include "plcBase.h"

// 
// If this is defined then sqlite will use this file.
// otherwise will use a RAM based database.
// Use a file when debugging.
//
#define DB_FILE "/tmp/plc2.db"  
using namespace std;


class plcMQTT : public plcBase 
{
    public:
        plcMQTT() ;
//        void setHost(string);
//        void setPort(int);

        bool initPlc() ;

        bool addIOPoint(string );               // Short name
        bool addIOPoint(string,string );        // Short name, topic
        bool addIOPoint(string,string, string );// Short name, topic, direction

        string getTopic(string );               // Short name
        string getValue(string );               // Short name
        bool setValue(string, string );         // Short name, value

        void plcRun();
        void setLogic( void (*f) (plcMQTT *));

        void Ld(string);
        void Out(string);
        void Outn(string);
    private:
        struct mosquitto *mosq;
        void  (*logic)(plcMQTT *);

        int keepalive=60;
        bool connect();
        bool dbSetup();
        bool sqlError(int, char *);
        void doStuff();

        sqlite3 *db;    // db pointer
        int con; // sqlite3 db connection
};
