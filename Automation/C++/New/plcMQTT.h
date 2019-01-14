
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
#include <semaphore.h>
#include "plcMQTT.h"
#include "plcBase.h"
// 
// If this is defined then sqlite will use this file.
// otherwise will use a RAM based database.
// Use a file when debugging.
//
#define DB_FILE "/tmp/plc2.db"  
using namespace std;

struct mqttData {
    sqlite3 *db; 
    sem_t mutex;
};

class plcMQTT : public plcBase {

    public:
        plcMQTT() ;
//        void setHost(string);
//        void setPort(int);

        void setDBname( string );
        string getDBname( );

        bool initPlc(string ) ;

        sem_t *getSem();
        
        int sqlCount(string sqlCmd);
        bool risingEdge(string);
        bool fallingEdge(string);

        bool addIOPoint(string );               // Short name
        bool addIOPoint(string,string );        // Short name, topic
        bool addIOPoint(string,string, string );// Short name, topic, direction

        string getTopic(string );               // Short name

        virtual string getValue(string );               // Short name
        virtual string getDirection(string );               // Short name
        virtual string getOldValue(string );               // Short name

        virtual bool setValue(string, string );         // Short name, string value
        // 
        // setOldValue, test only.
        //
        virtual bool setOldValue(string, string );         // Short name, string value

        virtual bool setBoolValue(string, bool );       // Short name, boolean value

        virtual bool getBoolValue(string );               // Short name
        virtual bool getBoolOldValue(string );               // Short name

        void plcRun();
        void setLogic( void (*f) (plcMQTT *));
        virtual bool plcEnd(int );              // Delay in ms

//        void Ld(string);
        void Ldr(string);
        void Ldf(string);
        
        void Out(string);
        void Outn(string);
    private:
        string dbName=":memory:";
        struct mqttData stuff;
        struct mosquitto *mosq;
        void  (*logic)(plcMQTT *);

        int keepalive=0;
        bool connect();
        bool dbSetup();
        bool sqlError(int, char *);
        void doStuff();

//        sqlite3 *db;    // db pointer
        int con; // sqlite3 db connection
};
