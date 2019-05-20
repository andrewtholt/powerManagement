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

/***********************************************************************
 *  Method: plcSocket::getValue
 *  Params: string shortName
 * Returns: bool
 * Effects: 
 ***********************************************************************/
string plcSocket::getValue(string shortName) {
    string res = "UNKNOWN";
    char buffer[64];

    bzero(buffer, sizeof(buffer));

    string cmd = "GET " + shortName + "\r\n";

    if( serverSock > 0) {
        int len  = write(serverSock, (char *)cmd.c_str(), (int)cmd.length());

        len = read(serverSock, buffer, sizeof(buffer));
    }

    return res;
}


/***********************************************************************
 *  Method: plcSocket::plcSocket
 *  Params: 
 * Effects: 
 ***********************************************************************/
plcSocket::plcSocket() {
    plcBase::verbose = false;
    plcSocket("/etc/mqtt/bridge.json");
}


/***********************************************************************
 *  Method: plcSocket::plcSocket
 *  Params: string cfgFile
 * Effects: 
 ***********************************************************************/
plcSocket::plcSocket(string cfgFile) {
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
 *  Method: plcSocket::setVerbose
 *  Params: bool flag
 * Returns: void
 * Effects: 
 ***********************************************************************/
void plcSocket::setVerbose(bool flag) {
    plcBase::verbose = flag;
}


/***********************************************************************
 *  Method: plcSocket::setValue
 *  Params: string shortName, string value
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
plcSocket::setValue(string shortName, string value)
{
}

void plcSocket::dump() {
    cout << "Socket Server:" + socketServer << endl;
    cout << "Socket Fd    :" << serverSock << endl;
    cout << "Port         :" << portNo << endl;
}

void plcSocket::setServerSock(int fd) {
    serverSock = fd;
}


