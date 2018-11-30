
#include "plcBase.h"
#include "plcMQTT.h"
#include <sqlite3.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;


void connect_callback(struct mosquitto *mosq, void *obj, int result) {
    printf("Connect_callback\n");
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    string sql;

    string payload;
    string topic;
    char *err_msg = NULL;

    sqlite3 *db = (sqlite3 *)obj;

    printf("================\n");
    printf("Message_callback\n");

    if(message->payloadlen) {

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
    //    hostName = "127.0.0.1";
    //    port = 1883;
}

bool plcMQTT::connect() {
    bool failFlag = true;

    return failFlag;
}

bool plcMQTT::initPlc() {
    bool failFlag = true;

    char clientid[24];
    failFlag = dbSetup();

    mosq = mosquitto_new(clientid, true, (void *)db);


    if(mosq) {
        failFlag = false;
        mosquitto_connect_callback_set(mosq, connect_callback);
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

        sqlite3_close(db);

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

#ifdef DB_FILE
    con = sqlite3_open( DB_FILE, &db );
#else
    con = sqlite3_open( ":memory:", &db );
#endif

    //    drop table if exists setting;

    int rc = sqlite3_exec(db, "drop table if exists iopoints;", 0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    if( failFlag == false ) {
        sql = "create table iopoints (";
        sql += "idx integer primary key autoincrement,";
        sql += "short_name varchar(32) not null," ;
        sql += "topic varchar(64) default 'LOCAL'," ;
        sql += "value varchar(32) default 'NONE'," ;
        sql += "direction varchar(32) default 'LOCAL'" ;
        sql += ");";

        if( verbose ) {
            cout << sql << endl;
        }

        rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
        failFlag=sqlError(rc, err_msg);
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

    rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
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

    rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
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

    rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    if( direction == "IN") {
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

    rc = sqlite3_prepare_v2( db, sql.c_str(), -1, &res, NULL);
    rc = sqlite3_step(res);

    tmp=(char *)sqlite3_column_text(res, 0);

    topic=tmp;

    return topic;

}

string plcMQTT::getValue(string shortName) {
    string sql;
    string value;
    sqlite3_stmt *res = NULL;
    char *tmp;

    char *err_msg = NULL;
    int rc;

    sql = "select value from iopoints where short_name = '" + shortName + "';";
    if(verbose) {
    cout << sql << endl;
    }

    rc = sqlite3_prepare_v2( db, sql.c_str(), -1, &res, NULL);
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

bool plcMQTT::setValue(string shortName, string value) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "update iopoints set value='"+value+"' where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
}

bool plcMQTT::setBoolValue(string shortName, bool v ) {
}

void plcMQTT::doStuff() {
}

void plcMQTT::plcRun() {
    //    mosquitto_loop_forever(mosq, -1, 1);

    while(true) {
        mosquitto_loop(mosq, -1, 1);

        if( logic != NULL) {
            logic(this);
        }
        usleep( 1000 );
    }
}

void plcMQTT::setLogic( void (*f) (plcMQTT *)) {
    cout << "setLogic" << endl;

    logic = f;
}

void plcMQTT::Ld(string symbol) {
    cout << "plcMQTT::Ld " << symbol ;

    bool v = getBoolValue( symbol );
    logicStack.push( v );

    cout << "   TOS: " << logicStack.top() << endl;
}
/*
void plcMQTT::Or(string symbol) {
    cout << "plcMQTT::Or " << symbol ;
    bool a;

    bool v = getBoolValue( symbol );

    a = fromStack() || v ;
    logicStack.push(a);
    cout << "   TOS: " << logicStack.top() << endl;
}
*/

/*
void plcMQTT::Andn(string symbol) {
    cout << "plcMQTT::Andn " << symbol ;

    bool a = fromStack() ;
    bool v = getBoolValue( symbol );

    a = a && (!v);
    logicStack.push(a);

    cout << "   TOS: " << logicStack.top() << endl;
}
*/


void plcMQTT::Out(string symbol) {
    cout << "plcMQTT::Out " << symbol << endl;
    bool a = fromStack() ;
    string topic ;
    string msg ;
    string old = getValue( symbol );

    topic = getTopic( symbol );
    msg   = boolToString(a);

    if( topic != "LOCAL")  {
        // publish
        if( old != msg ) {
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
    cout << "plcMQTT::Outn " << symbol << endl;
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
