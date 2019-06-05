#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <sys/poll.h>
#include <pthread.h>
#include <mosquitto.h>

#include <mysql/mysql.h>
#include <pwd.h>
#include <grp.h>

#include <thread>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

json config ;
bool verbose=false;
string dbName;

string stateToLogic(string st) {
    string v="OFF";
    
    if( st == "ON" || st == "TRUE") {
        v = "ON";
    }
    
    if( st == "OFF" || st == "FALSE") {
        v = "OFF";
    }
    return v;
}

void finish_with_error(MYSQL *con) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *mqttMsg) {
    MYSQL *con;
    con=mysql_init(NULL);
    string sqlCmd;
    
    cout << "\non_message" << endl;
    
    string topic = (char *)mqttMsg->topic;
    string state = (char *)mqttMsg->payload;
    
    cout << "Topic : " << topic << endl;
    cout << "Msg   : " << state << endl;
    
    if ( con == NULL ) {
        cerr << "DB Connect failed" << endl;
        exit(3);
    }
    
    if (mysql_real_connect(con, dbName.c_str(), "automation", "automation", "automation", 0, NULL, 0) == NULL) {
        finish_with_error(con);
    }
    // state = (msg.payload).decode("utf-8")
    
    sqlCmd = "select state, direction, data_type from mqttQuery where topic = '" + topic + "';";
    
    cout << sqlCmd << endl;
    
    MYSQL_RES *result = NULL;
    int mysqlStatus=mysql_query(con, sqlCmd.c_str());
    if( mysqlStatus) {
        cerr << "SQL Error" << endl;
    } else {
//        result = mysql_store_result(con);
        result = mysql_use_result(con);
        MYSQL_ROW row;
        
        int numFields = mysql_field_count(con);
        
        cout << "Number of fields = " << numFields << endl;
        
        row = mysql_fetch_row( result );
        
        string dbState = row[0];
        string dbDirection = row[1];
        string dataType = row[2];
        string devState;
        
        cout << "=======" << endl;
        cout << "Topic     :" + topic << endl;
        cout << "Direction :" + dbDirection  << endl;
        cout << "Type      :" + dataType  << endl;
        cout << "DB State  :" + dbState   << endl;
        cout << "MQTT State:" + state   << endl;
        cout << "=======" << endl;
        
        
        if( dataType == "BOOL") {
            devState = stateToLogic(state);
        } else if (dataType == "STRING") {
            devState = state;
        }
        
        if( dbDirection == "OUT" ) {
            if(devState != dbState) {
                // Publish
                mosquitto_publish(mosq, NULL, (char *)topic.c_str(), dbState.length() , (char *)dbState.c_str(), 1,true) ;
            }
        } else if ( dbDirection == "IN" ) {
            // Update the db
            sqlCmd = "update mqtt set state = '" + devState + "' where topic = '" + topic + "';";
            cout << sqlCmd << endl;
            
            if( mysql_query(con, sqlCmd.c_str())) {
                cerr << "SQL Error" << endl;
            }
        }
    }
    mysql_free_result(result);
    result = NULL;
    
    mysql_close(con);
}

void on_connect(struct mosquitto *mosq, void *obj, int result) {
    MYSQL *con;
    con=mysql_init(NULL);
    string sqlCmd;
    
    if ( con == NULL ) {
        cerr << "DB Connect failed" << endl;
        exit(3);
    }
    
    if (mysql_real_connect(con, dbName.c_str(), "automation", "automation", "automation", 0, NULL, 0) == NULL) {
        finish_with_error(con);
    }
    sqlCmd="select name,direction,topic,state from mqttQuery ;";
    cout << sqlCmd << endl;
    
    if( mysql_query(con, sqlCmd.c_str())) {
        cerr << "SQL Error" << endl;
    } else {
        MYSQL_RES *result = mysql_store_result(con);
        MYSQL_ROW row;
        
        while ((row = mysql_fetch_row(result))) {
            cout << row[0] << endl;
            cout << row[2] << endl;
            
            mosquitto_subscribe(mosq,NULL, row[2],1);
        }
        mysql_free_result(result);
        result = NULL;
    }
    mysql_close(con);
}

void on_disconnect(struct mosquitto *mosq, void *obj, int result) {
}


void usage(string name) {
    cout << endl;
    cout << "Usage: " + name + " -h|-c <cfg file> -v -f -p <port>" << endl;
    cout << "\t-h\t\tHelp." << endl;
    cout << "\t-c <cfg>\tGet config from specified file." << endl;
    cout << "\t-v\t\tVerbose." << endl;
    cout << "\t-f\t\tRun in foreground." << endl;
}


int main(int argc, char *argv[]) {
    
    int opt;
    
    int sockfd; // Socket file descriptor
    string cfgFile = "/etc/mqtt/bridge.json";
    
    sockaddr_in serv_addr; // Server address
    
    bool fg = false;
    
    string svcName = basename(argv[0]) ;
    
    while( (opt = getopt(argc, argv, "c:fhv")) != -1) {
        switch(opt) {
            case 'c':
                cfgFile = optarg;
                break;
            case 'f':
                fg=true;
                break;
            case 'h':
                usage(svcName);
                exit(1);
                break;
            case 'v':
                verbose = true;
                break;
        }
    }
    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;
        exit(2);
    }
    
    ifstream cfgStream( cfgFile );
    
    config = json::parse(cfgStream);
    
    dbName = config["database"]["name"];
    
    /*
     *    con=mysql_init(NULL);
     * 
     *    if ( con == NULL ) {
     *        cerr << "DB Connect failed" << endl;
     *        exit(3);
}

if (mysql_real_connect(con, dbName.c_str(), "automation", "automation", "automation", 0, NULL, 0) == NULL) {
    finish_with_error(con);
}
*/
    
    string mosquittoHost = config["local"]["name"];
    string mqttPortString = config["local"]["port"];
    
    int mosquittoPort = stoi( mqttPortString );
    int rc;
    int keepalive=0;
    
    struct mosquitto *mosq ;
    mosq = mosquitto_new(svcName.c_str(), true, NULL);
    
    if( mosq) {
        rc=mosquitto_connect(mosq, mosquittoHost.c_str(), mosquittoPort, keepalive);
        
        if(verbose) {
            cout << "Registering callbacks ..." << endl;
        }
        
        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_disconnect_callback_set(mosq, on_disconnect);
        mosquitto_message_callback_set(mosq, on_message);
        
        if(verbose) {
            cout << "Done ..." << endl;
        }
    }
    
    bool iamRoot=false;
    mosquitto_loop_forever(mosq, 0,1);
}

