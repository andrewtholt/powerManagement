/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//plcSocket.cpp
 *   DATE: Wed May 15 18:04:49 2019
 *  DESCR: 
 ***********************************************************************/
#include "plcBase.h"
#include "plcSocket.h"
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>

#include <fstream>

using json = nlohmann::json;

using namespace std;

inline string &ltrim(string& s, const char* t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline string &rtrim(string& s, const char* t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

inline string &trim(string& s, const char* t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t);
}


/***********************************************************************
 *  Method: plcSocket::getValue
 *  Params: string shortName
 * Returns: bool
 * Effects: 
 ***********************************************************************/
string plcSocket::getValue(string shortName) {
    string res = "UNKNOWN";
    string tmp;
    char buffer[64];

    bzero(buffer, sizeof(buffer));

    string cmd = "GET " + shortName + "\r\n";

    if( serverSock > 0) {
        int len  = write(serverSock, (char *)cmd.c_str(), (int)cmd.length());

        len = read(serverSock, buffer, sizeof(buffer));
        tmp=buffer;
        string res = rtrim(tmp);
    }

    return res;
}

bool plcSocket::getBoolValue(string shortName) {
    string tmp = getValue(shortName);

    string res = rtrim(tmp);
    bool v = false ;

    if ( res == "ON" || res == "TRUE" ) {
        v = true;
    } else {
        v = false ;
    }

    return v;
}

const void plcSocket::plcSetup(string cfgFile) {
    plcBase::verbose = false;
    struct sockaddr_in si_other;
    socklen_t slen=sizeof(si_other);
    int sock;

    cout << "Config :" + cfgFile << endl;

    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;

        instanceFailed = true;

    } else {
        int status=-1;
        ifstream cfgStream( cfgFile );

        config = json::parse(cfgStream);

        socketServer = config["socket"]["name"];
        string portNoString =  config["socket"]["port"] ;

        portNo = stoi( portNoString );

        sock = socket(AF_INET , SOCK_STREAM , 0);
        if ( sock == -1) {
            instanceFailed = true;
        } else {
            memset((char *) &si_other, 0, sizeof(si_other));
            si_other.sin_addr.s_addr = inet_addr( (char *)socketServer.c_str() );
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(portNo);

            if ( connect(sock , (struct sockaddr *)&si_other , sizeof(si_other)) < 0) {
                cerr << "Connect failed" << endl;
                instanceFailed = true;
            } else {
                instanceFailed = false;
                serverSock=sock;
            }
        }
    }
}

/***********************************************************************
 *  Method: plcSocket::plcSocket
 *  Params: 
 * Effects: 
 ***********************************************************************/
plcSocket::plcSocket() {
    plcSetup("/etc/mqtt/bridge.json");
}


/***********************************************************************
 *  Method: plcSocket::plcSocket
 *  Params: string cfgFile
 * Effects: 
 ***********************************************************************/
plcSocket::plcSocket(string cfgFile) {
    plcSetup(cfgFile);
}


/***********************************************************************
 *  Method: plcSocket::setVerbose
 *  Params: bool flag
 * Returns: void
 * Effects: 
 ***********************************************************************/
/*
   void plcSocket::setVerbose(bool flag) {
   plcBase::verbose = flag;
   }
   */

/***********************************************************************
 *  Method: plcSocket::setValue
 *  Params: string shortName, string value
 * Returns: void
 * Effects: 
 ***********************************************************************/
void plcSocket::setValue(string shortName, string value) {
    char buffer[64];
    string cmd = "SET " + shortName + " " + value + "\r" ;

    int len = write(serverSock, cmd.c_str(), cmd.length() );
    fsync( serverSock );
    //
    //  This read could be used to verify that the write took place.
    //
    len = read(serverSock, buffer, sizeof(buffer));
}

void plcSocket::setBoolValue(string shortName, bool flag) {
    string value = (flag) ? "ON" : "OFF";

    setValue(shortName, value);

}

void plcSocket::dump() {
    cout << "Socket Server:" + socketServer << endl;
    cout << "Socket Fd    :" << serverSock << endl;
    cout << "Port         :" << portNo << endl;
}

void plcSocket::setServerSock(int fd) {
    serverSock = fd;
}

void plcSocket::Ld(string symbol) {
    if(verbose) {
        cout << "plcBase::Ld " << symbol ;
    }

    Push( getBoolValue(symbol));

    if(verbose) {
        cout << "   TOS: " << logicStack.back() << endl;
    }
}

void plcSocket::Out(string symbol) {

    setBoolValue(symbol, Pop());
}

