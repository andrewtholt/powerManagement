
#include "plcDatabase.h"
#include "plcBase.h"
#include <iostream>

using namespace std;

plcDatabase::plcDatabase() {
    dbSetup();
}

plcDatabase::plcDatabase(string name) {
    setDBname(name);
    dbSetup();
}

bool plcDatabase::sqlError(int rc, char *err_msg ) {
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

bool plcDatabase::dbSetup() {
    bool failFlag = true;
    char *err_msg = NULL;
    string sql;

    con = sqlite3_open(dbName.c_str(), &db );
    //    drop table if exists setting;

    int rc = sqlite3_exec(db, "drop table if exists iopoints;", 0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    if( failFlag == false ) {
        sql = "drop table if exists iopoints";
        rc = sqlite3_exec(db, sql.c_str(),0,0,&err_msg);
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

        rc = sqlite3_exec(db,sql.c_str(),0,0,&err_msg);
        failFlag |=sqlError(rc, err_msg);
    }

    return failFlag;
}

string plcDatabase::getValue(string shortName) {
    string sql;
    string value;
    sqlite3_stmt *res = NULL;
    char *tmp;

    char *err_msg = NULL;
    int rc;

    sql = "select value from iopoints where short_name = '" + shortName + "';";

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
bool plcDatabase::setValue(string shortName, string value) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "update iopoints set value='"+value+"' where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec( db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
}

string plcDatabase::getOldValue(string shortName) {
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

bool plcDatabase::setOldValue(string shortName, string value) {
    string sql;
    bool failFlag=true;
    char *err_msg = NULL;
    int rc;

    sql = "update iopoints set oldvalue='"+value+"' where short_name = '" + shortName + "';";
    if(verbose) {
        cout << sql << endl;
    }

    rc = sqlite3_exec( db,sql.c_str(),0,0,&err_msg);
    failFlag=sqlError(rc, err_msg);

    return failFlag;
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
