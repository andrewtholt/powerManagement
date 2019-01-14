
#include "plcDatabase.h"
#include "plcBase.h"

plcDatabase::plcDatabase() {
}


string plcDatabase::getValue(string symbol) {
    
    string ret="";
    
    return ret;
}

void plcDatabase::setDBname(string db) {
    static bool set = false;

    if(set == false) {
        dbName = db;
        set = true;
    }
}

string plcDatabase::getDBname() {
    return dbName;
}
