/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//interp.cpp
 *   DATE: Mon Sep  2 09:10:25 2019
 *  DESCR: 
 ***********************************************************************/
#include <algorithm>
#include <vector>
#include "interp.h"


using namespace std;

string interp::setRemoteVariable(string name, string value) {
    string ret="<NOT FOUND>";

    string sqlCmd = "update io_point set old_state=state, state = '" + value + "' where name='" + name + "';";
    cout << sqlCmd << endl;
    int rc = mysql_query(conn, sqlCmd.c_str());

    return value;
}

string interp::getRemoteVariable(string name) {
    string ret="<NOT FOUND>";

    string sqlCmd = "select state from io_point where name='" + name + "';";

    int rc = mysql_query(conn, sqlCmd.c_str());

    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);

    ret = row[0];

    return ret;
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

    cout << "GET:" << endl;
    cout << c.size() << endl;

    if ( c.size() >= 2 ) { 
        cout << "\t[0]:" + c[0] << endl;
        cout << "\t[1]:" + c[1] << endl;

        string tmp = c[1];
        if ( c[1].at(0) == '$' ) {
            cout << "Local" << endl;
            tmp.erase(tmp.begin());
            cout << tmp << endl;
            ret = localVariable[tmp];
        } else {
            ret = getRemoteVariable(tmp);
        }
    }
    cout << "===" << endl;
    return ret;
}

std::string interp::undefinedCmd(std::vector<string>) {
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
    } else {
        out = "<UNKNOWN>";
    }

    return out;
}


