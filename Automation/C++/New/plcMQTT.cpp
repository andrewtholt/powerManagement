
#include "plcBase.h"
#include "plcMQTT.h"
#include <sqlite3.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#include <mosquitto.h>

using namespace std;

// sem_t mutex;


void connect_callback(struct mosquitto *mosq, void *obj, int result) {
    printf("Connect_callback %d\n", result);
}

void disconnect_callback(struct mosquitto *mosq, void *obj, int result) {
    fprintf(stderr, "Disonnect_callback %d\n", result);
    exit(1);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    string sql;

    string payload;
    string topic;
    char *err_msg = NULL;
    int sval;
    int rc=0;
    
    struct mqttData *data;
    
    data = (struct mqttData *)obj;

//    sqlite3 *db = (sqlite3 *)obj;
    sqlite3 *db = (sqlite3 *)data->db;

    printf("================\n");
    printf("Message_callback\n");

    if(message->payloadlen) {
        sem_t *mutex;
        
        mutex = &(data->mutex);
//        sem_post( &mutex );
        rc = sem_post( mutex );

//        rc = sem_getvalue(&mutex, &sval);
        rc = sem_getvalue(mutex, &sval);

        printf("message callback sem= %d\n", sval);

        cout << "Topic   " << message->topic << endl;
        cout << "Message " << (char *)message->payload << endl;
        cout << "================" << endl;

        payload = (char *)message->payload;
        topic = message->topic ;
        sql = "update iopoints set value='" + payload + "' ";
        sql += "where topic = '" + topic + "';";

        int rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
        cout << sql << endl;
    }

}

void subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *    granted_qos) {
    printf("Subscribe_callback\n");
    printf("Subscribed (mid: %d): %d\n", mid, granted_qos[0]);
}

plcMQTT::plcMQTT() {
    printf("plcMQTT\n");
    logic = NULL;
    //    hostName = "127.0.0.1";
    //    port = 1883;
}

void plcMQTT::setDBname(string db) {
    static bool set = false;

    if(set == false) {
        dbName = db;
        set = true;
    }
}

string plcMQTT::getDBname() {
    return dbName;
}

bool plcMQTT::connect() {
    bool failFlag = true;

    return failFlag;
}

sem_t *plcMQTT::getSem()  {
    return &stuff.mutex;
}

bool plcMQTT::initPlc(string clientId) {
    bool failFlag = true;

    if(verbose) {
        cout << "plcMQTT::initPlc" << endl;
    }

    //     char clientid[24];

    failFlag = dbSetup();

//    sem_init(&mutex, 0, 0);
    sem_init(&(stuff.mutex), 0, 0);

    mosq = mosquitto_new(clientId.c_str(), true, (void *)&stuff);

    if(mosq) {
        failFlag = false;
        mosquitto_connect_callback_set(mosq, connect_callback);
        mosquitto_disconnect_callback_set(mosq, disconnect_callback);
        mosquitto_message_callback_set(mosq, message_callback);
    } else {
        failFlag = true;
        return failFlag;
    }

    if(mosquitto_connect(mosq, hostName.c_str(), port, keepalive)) {
        fprintf(stderr, "Unable to connect.\n");
        failFlag = true ;
    }    

    //    mosquitto_loop_forever(mosq, -1, 1);
    return failFlag;
}

bool plcMQTT::sqlError(int rc, char *err_msg ) {
    bool failFlag = true;

    if( rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);

        sqlite3_close(stuff.db);

        failFlag = true;
    } else {
        failFlag = false;
    }
    return failFlag;
}

bool plcMQTT::dbSetup() {
    bool failFlag = true;
    char *err_msg = NULL;
    string sql;

    /*
#ifdef DB_FILE
    con = sqlite3_open( DB_FILE, &stuff.db );
#else
    con = sqlite3_open( ":memory:", &stuff.db );
#endif
*/
    con = sqlite3_open(dbName.c_str(), &stuff.db );
    //    drop table if exists setting;

    int rc = sqlite3_exec(stuff.db, "drop table if exists iopoints;", 0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    if( failFlag == false ) {
        sql = "drop table if exists iopoints";
        rc = sqlite3_exec(stuff.db, sql.c_str(),0,0,&err_msg);
        failFlag=sqlError(rc, err_msg);

        sql = "create table iopoints (";
        sql += "idx integer primary key autoincrement,";
        sql += "short_name varchar(32) not null unique," ;
        sql += "topic varchar(64) default 'LOCAL'," ;
        sql += "value varchar(32) default 'OFF'," ;
        sql += "oldvalue varchar(32) default 'OFF'," ;
        sql += "direction varchar(32) default 'LOCAL'" ;
        sql += ");";

        if( verbose ) {
            cout << sql << endl;
        }

        rc = sqlite3_exec(stuff.db,sql.c_str(),0,0,&err_msg);
        failFlag |=sqlError(rc, err_msg);
    }

    return failFlag;
}

bool plcMQTT::addIOPoint(string shortName) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "insert into iopoints (short_name) values ('" + shortName + "');";

    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec(stuff.db, sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
}

bool plcMQTT::addIOPoint(string shortName, string topic) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "insert into iopoints (short_name, topic) values ('" + shortName + "',";
    sql += "'" + topic + "'";
    sql += ");";

    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec(stuff.db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
}

bool plcMQTT::addIOPoint(string shortName, string topic, string direction) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "insert into iopoints (short_name, topic, direction) values ('" + shortName + "',";
    sql += "'" + topic + "',";
    sql += "'" + direction + "'";
    sql += ");";

    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec(stuff.db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    if( direction == "IN") {
        cout << "Subscribe " << topic << endl;
        mosquitto_subscribe(mosq, NULL, topic.c_str(),0);
    }

    return failFlag;
}

string plcMQTT::getTopic(string shortName) {
    string sql;
    string topic;

    sqlite3_stmt *res = NULL;
    char *tmp;

    char *err_msg = NULL;
    int rc;

    sql = "select topic from iopoints where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_prepare_v2( stuff.db, sql.c_str(), -1, &res, NULL);
    rc = sqlite3_step(res);

    tmp=(char *)sqlite3_column_text(res, 0);

    topic=tmp;

    return topic;

}

string plcMQTT::getDirection(string shortName) {
    string sql;
    string dir;
    sqlite3_stmt *res = NULL;
    char *tmp;

    char *err_msg = NULL;
    int rc;

    sql = "select direction from iopoints where short_name = '" + shortName + "';";
    /*
       if(verbose) {
       cout << sql << endl;
       }
       */

    rc = sqlite3_prepare_v2( stuff.db, sql.c_str(), -1, &res, NULL);
    rc = sqlite3_step(res);

    tmp=(char *)sqlite3_column_text(res, 0);

    if( tmp ) {
        dir=tmp;
    } else {
        dir="";
    }
    return dir;
}

string plcMQTT::getValue(string shortName) {
    string sql;
    string value;
    sqlite3_stmt *res = NULL;
    char *tmp;

    char *err_msg = NULL;
    int rc;

    sql = "select value from iopoints where short_name = '" + shortName + "';";
    /*
       if(verbose) {
       cout << sql << endl;
       }
       */

    rc = sqlite3_prepare_v2( stuff.db, sql.c_str(), -1, &res, NULL);
    rc = sqlite3_step(res);

    tmp=(char *)sqlite3_column_text(res, 0);

    if( tmp ) {
        value=tmp;
    } else {
        value="";
    }
    return value;
}

string plcMQTT::getOldValue(string shortName) {
    string sql;
    string value;
    sqlite3_stmt *res = NULL;
    char *tmp;

    char *err_msg = NULL;
    int rc;

    sql = "select oldvalue from iopoints where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_prepare_v2( stuff.db, sql.c_str(), -1, &res, NULL);
    rc = sqlite3_step(res);

    tmp=(char *)sqlite3_column_text(res, 0);

    if( tmp ) {
        value=tmp;
    } else {
        value="";
    }

    return value;
}

bool plcMQTT::getBoolValue( string shortName ) {

    return stringToBool( getValue( shortName ));
}

bool plcMQTT::getBoolOldValue( string shortName ) {

    return stringToBool( getOldValue( shortName ));
}

bool plcMQTT::setValue(string shortName, string value) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "update iopoints set value='"+value+"' where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec( stuff.db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
}

bool plcMQTT::setOldValue(string shortName, string value) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "update iopoints set oldvalue='"+value+"' where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec( stuff.db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
}

bool plcMQTT::setBoolValue(string shortName, bool v ) {
}

void plcMQTT::doStuff() {
}

void plcMQTT::plcRun() {
    if( verbose ) {
        cout << "plcMQTT::plcRun" << endl;
    }

    mosquitto_loop_start(mosq);

    while( logic != NULL) {
        logic(this);

        usleep( 1000 );
    }
}

void plcMQTT::setLogic( void (*f) (plcMQTT *)) {
    cout << "setLogic" << endl;

    logic = f;
}

/*
 *   void plcMQTT::Ld(string symbol) {
 *   cout << "plcMQTT::Ld " << symbol ;
 * 
 *   bool v = getBoolValue( symbol );
 *   logicStack.push( v );
 * 
 *   cout << "   TOS: " << logicStack.top() << endl;
 *   }
 * 
 *   void plcMQTT::Or(string symbol) {
 *   cout << "plcMQTT::Or " << symbol ;
 *   bool a;
 * 
 *   bool v = getBoolValue( symbol );
 * 
 *   a = fromStack() || v ;
 *   logicStack.push(a);
 *   cout << "   TOS: " << logicStack.top() << endl;
 *   }
 */

bool plcMQTT::risingEdge(string symbol) {
    string sql="short_name='" + symbol + "' and value='ON' and oldvalue='OFF';";

    int c = sqlCount( sql );
    bool risingEdge = ( c == 1 ) ? true : false ;

    return risingEdge;
}

bool plcMQTT::fallingEdge(string symbol) {
    string sql="short_name='" + symbol + "' and value='OFF' and oldvalue='ON';";

    int c = sqlCount( sql );
    bool fallingEdge = ( c == 1 ) ? true : false ;

    return fallingEdge;
}

void plcMQTT::Ldr(string symbol) {
    if(verbose) {
        cout << "plcMQTT::Ldr " << symbol ;
    }

    logicStack.push(risingEdge(symbol));

    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}

void plcMQTT::Ldf(string symbol) {
    if(verbose) {
        cout << "plcMQTT::Ldf " << symbol ;
    }

    logicStack.push(fallingEdge(symbol));

    if(verbose) {
        cout << "   TOS: " << logicStack.top() << endl;
    }
}


void plcMQTT::Out(string symbol) {
    static bool first = true;
    if(verbose) {
        cout << "plcMQTT::Out " << symbol << endl;
    }

    bool a = fromStack() ;
    string topic ;
    string dir ;

    string msg = getValue( symbol );;
    string old = getOldValue( symbol );

    topic = getTopic( symbol );
    dir = getDirection( symbol );

    msg   = boolToString(a);

    //    if( topic != "LOCAL")  {
    if( dir != "LOCAL")  {
        // publish
        cout << "Publish Topic " << topic << endl;
        cout << "\tOld = " << old << endl;
        cout << "\tNew = " << msg << endl;

        if( (old != msg ) || first == true) {
            first = false;
            cout << "Publish Topic " << topic << endl;
            mosquitto_publish(mosq,NULL,topic.c_str(), strlen(msg.c_str()), msg.c_str(), 0,true);
        }
    } 
    // set local db value

    setValue( symbol, msg);

    while( !logicStack.empty() ) {
        logicStack.pop();
    }
}

void plcMQTT::Outn(string symbol) {
    if(verbose) {
        cout << "plcMQTT::Outn " << symbol << endl;
    }
    bool a = !fromStack() ;
    string topic ;

    topic = getTopic( symbol );

    if( topic != "LOCAL")  {
    }

    setValue( symbol, boolToString( a ));

    while( !logicStack.empty() ) {
        logicStack.pop();
    }
}

bool plcMQTT::plcEnd(int ms) {
    bool failFlag=true;
    char *err_msg = NULL;
    static int count=0;

    if(verbose) {
        cout << "plcMQTT::plcEnd" << endl;
    }

    string sql="update iopoints set oldvalue=value;";
    int rc = sqlite3_exec( stuff.db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    if(verbose) {
        int sval;
        int rc = sem_getvalue( &(stuff.mutex), &sval);
        cout << "plcMQTT::plcEnd=" << sval << endl;
    }
    cout << "plcMQTT::plcEnd " << count << endl;

    if( ms > 0) {
        struct timespec ts;

        ts.tv_nsec+=ms * 100000;
        if (ts.tv_nsec>=1000000000) {
            ts.tv_sec+=1;
            ts.tv_nsec-=1000000000;
        }

        sem_timedwait( &(stuff.mutex), &ts );
    } else {

        sem_wait( &(stuff.mutex) );
    }

    failFlag |=plcBase::plcEnd(ms);

    count++;
    return failFlag;

}

int plcMQTT::sqlCount(string sqlCmd) {
    int count=-1;

    sqlite3_stmt *res = NULL;

    string sql = "select count(*) from iopoints where ";

    sql += sqlCmd ;
    sql += ";";

    if(verbose) {
        cout << sql << endl;
    }

    int rc = sqlite3_prepare_v2( stuff.db, sql.c_str(), -1, &res, NULL);
    rc = sqlite3_step(res);

    char *tmp=(char *)sqlite3_column_text(res, 0);

    count = atoi(tmp);

    return count;
}









