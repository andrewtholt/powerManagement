/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//interp.cpp
 *   DATE: Mon Sep  2 09:10:25 2019
 *  DESCR: 
 ***********************************************************************/
#include <algorithm>
#include <vector>
#include <nlohmann/json.hpp>
#include <time.h>

#include "interp.h"

using json = nlohmann::json;

using namespace std;

extern uint64_t startTime;  // system start time.

void interp::setLocal(string key, string value) {
    localVariable[key] = value;
}

void interp::setClientIP(string ip) {
    clientIP=ip;
}

string interp::getClientIP() {
    return clientIP;
}

void interp::dump() {
    cout << "Client IP   : " << clientIP << endl;
    cout << "Locals " << endl;

    //    cout << localVariable << endl;
}

map<string, string> interp::sqlQuery(string table, string key){
    map<string,string> data;
    MYSQL_FIELD *field;

    string sqlCmd = "select * from " + table + " where name='" + key + "';";

    cout << sqlCmd << endl;

    int rc = mysql_query(conn, sqlCmd.c_str());

    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);

    unsigned int num_fields = mysql_num_fields(result);
    unsigned int num_rows   = mysql_num_rows(result);

    if( num_rows  > 0 ) {
        char *headers[num_fields];
        for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
            data[ field->name ] = row[i];
        }
    }
    mysql_free_result( result );

    return data;
}

void interp::setDestQ( string qName ) {
    struct mq_attr attr;

    destQ = qName;

    if( dest >=0 ) {
        mq_close(dest);
    }

    //    dest = mq_open(destQ.c_str(), O_WRONLY|O_CREAT,0664, &attr);
    dest = mq_open(destQ.c_str(), O_WRONLY|O_CREAT,0664, NULL);
    if( dest == -1) {
        perror("mq_open");
        exit(2);
    }
}

string interp::setRemoteVariable(string name, string value) {
    map<string,string> data;
    map<string,string> pointData;

    string ret="<NOT FOUND>";

    string sqlCmd = "update io_point set old_state=state, state = '" + value + "' where name='" + name + "';";
    cout << sqlCmd << endl;
    int rc = mysql_query(conn, sqlCmd.c_str());

    data = sqlQuery( "io_point",name);

    string dataType = data["io_type"];

    transform(dataType.begin(), dataType.end(), dataType.begin(), ::tolower);

    pointData = sqlQuery( dataType + "Query", name );

    if( dest > -1) {
        char outBuffer[512];
        bzero(outBuffer,sizeof(outBuffer));

        json outMap(pointData);
        string toDispatch = outMap.dump() ;

        strncpy(outBuffer, toDispatch.c_str(), toDispatch.length());

        if (mq_send(dest, outBuffer, strlen(outBuffer), 0) < 0) {
            perror("setRemoteVariable:mq:send ");
        }
    }
    /* 
       string sqlCmd = "update io_point set old_state=state, state = '" + value + "' where name='" + name + "';";
       cout << sqlCmd << endl;
       int rc = mysql_query(conn, sqlCmd.c_str());
       */

    return value;
}

map<string,string> interp::getRemoteVariable(string name) {
    map<string,string> data;
    MYSQL_FIELD *field;

    data = sqlQuery( "io_point",name);

    return data;
}

/***********************************************************************
 *  Method: interp::test
 *  Params: std::string, std::string
 * Returns: std::string
 * Effects: 
 ***********************************************************************/
std::string interp::Ping(std::vector<string>) {
    string ret="Pong!";
    return ret;
}

std::string interp::Close(std::vector<string>) {
    string ret="CLOSED";
    return ret;
}

std::string interp::Set(std::vector<string> c) {
    string ret="<UNKNOWN>";

    if (c.size() == 3 ) {
        if ( c[1].at(0) == '$' ) {
            string tmp = c[1];
            tmp.erase(tmp.begin());

            localVariable[tmp] = c[2];
            ret=c[2];
        } else {
            ret = setRemoteVariable(c[1], c[2]);
        }
    }
    return ret;
}

std::string interp::Get(std::vector<string> c) {
    string ret="<UNKNOWN>";

    uint64_t uptime;

    time_t now = time(NULL);
    uptime = now - startTime;
    localVariable["UPTIME"] = to_string( uptime );

    cout << "GET:" << endl;
    cout << c.size() << endl;

    if ( c.size() >= 2 ) { 
        cout << "\t[0]:" + c[0] << endl;
        cout << "\t[1]:" + c[1] << endl;

        string tmp = c[1];
        tmp.erase(tmp.begin());
        cout << tmp << endl;

        if ( c[1].at(0) == '$' ) {
            // Local variable
            cout << "Local" << endl;
            ret = localVariable[tmp];
        } else if ( c[1].at(0) == '^' ) {
            // Global variable
            cout << "Global" << endl;
            ret = globalVariable[tmp];
        } else {
            map<string,string> data = getRemoteVariable(tmp);
            ret = data["state"];
        }
    }
    cout << "===" << endl;
    return ret;
}

string interp::Toggle(vector<string> c) {
    string state = Get( c );

    if( state == "ON" ) {
        state = "OFF";
    } else if( state == "OFF" ) {
        state = "ON";
    }

    c.push_back( state );

    return Set( c );
}

string interp::Reset(vector<string> c) {
    string ret = "<NOTHING>";

    int cLen = c.size();

    string sqlCmd;

    if( cLen == 1 ) {
        sqlCmd = "update io_point set old_state=state ;";
    } else if (cLen == 2 ) {
        sqlCmd = "update io_point set old_state=state where name='" + c[1] + "';";
    }

    cout << sqlCmd << endl;

    int rc = mysql_query(conn, sqlCmd.c_str());
    if( rc == 0) {
        ret = "OK";
    } else {
        ret="ERROR:SQL";
    }

    return ret;
}

string interp::Cold(vector<string> c) {
    string ret = "<NOTHING>";

    if ( clientIP == "127.0.0.1") {
        int clientCnt = stoi(globalVariable["CLIENTS"]);
        if( clientCnt == 1) {
            string sqlCmd = "update io_point set state='OFF', old_state='OFF' where state = 'ON' or state='OFF';";
            int rc = mysql_query(conn, sqlCmd.c_str());
            if( rc == 0) {
                ret = "OK";
            } else {
                ret="ERROR:SQL";
            }
        } else {
            ret = "BUSY";
        }
    } else {
        ret = "NO_PERM";
    }

    return ret;
}


std::string interp::undefinedCmd(std::vector<string> c) {
    string ret="<UNIMPLEMENTED>";
    return ret;
}


/***********************************************************************
 *  Method: interp::interp
 *  Params: 
 * Effects: 
 ***********************************************************************/
interp::interp(MYSQL *c) {
    conn = c;

    localVariable["VERSION"] = VERSION;
}
interp::interp() {
    /*
       cmd["PING"] = &Ping; 
       cmd["CLOSE"] = &Close; 
       cmd["COLD"] = &undefinedCmd; 
       cmd["RESET"] = &undefinedCmd; 

       cmd["GET"] = &interp::Get;
       cmd["TOGGLE"] = &undefinedCmd; 

       cmd["SET"] = &undefinedCmd; 
       */
}


/***********************************************************************
 *  Method: interp::runCmd
 *  Params: string
 * Returns: void
 * Effects: 
 ***********************************************************************/
string interp::runCmd(vector<string> c) {
    string r="";
    string out;

    if( c[0] == "PING" ) {
        out=Ping(c);
    } else if ( c[0] == "CLOSE" ) {
        out=Close(c);
    } else if ( c[0] == "GET" ) {
        out=Get(c);
    } else if ( c[0] == "SET" ) {
        out=Set(c);
    } else if ( c[0] == "RESET" ) {
        out=Reset(c);
    } else if ( c[0] == "TOGGLE" ) {
        out=Toggle(c);
    } else if ( c[0] == "COLD" ) {
        out=Cold( c );
    } else {
        out = "<UNKNOWN>";
    }

    return out;
}


