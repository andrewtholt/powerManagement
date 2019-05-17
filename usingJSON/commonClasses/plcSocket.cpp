/***********************************************************************
 * AUTHOR: Andrew Holt <andrewh>
 *   FILE: .//plcSocket.cpp
 *   DATE: Wed May 15 18:04:49 2019
 *  DESCR: 
 ***********************************************************************/
#include "plcBase.h"
#include "plcSocket.h"

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

    return res;
}


/***********************************************************************
 *  Method: plcSocket::plcSocket
 *  Params: 
 * Effects: 
 ***********************************************************************/
plcSocket::plcSocket() {
    plcBase::verbose = false;
    plcSocket("/etc/mqtt/bridge.ini");
}


/***********************************************************************
 *  Method: plcSocket::plcSocket
 *  Params: string cfgFile
 * Effects: 
 ***********************************************************************/
plcSocket::plcSocket(string cfgFile) {

    cout << "Config :" + cfgFile << endl;

    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;

        instanceFailed = true;

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


