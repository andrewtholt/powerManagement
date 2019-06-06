#include <mysql.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

json config ;
bool verbose=false;
MYSQL *conn;

map<string,string> getFromMqttQuery(string name) {
// void getFromMqttQuery(string name) {
    map<string,string> data;
    
    MYSQL_FIELD *field;
    string sqlCmd = "select * from mqttQuery where name='" + name + "';";
    
    int rc = mysql_query(conn, sqlCmd.c_str());
    
    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);
    
    unsigned int num_fields = mysql_num_fields(result);
    
    cout << rc << endl;
    char *headers[num_fields];
    for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
        cout << field->name ;
        cout << ":" << row[i] << endl;
        
        data[ field->name ] = row[i];
        
    }
    
    mysql_free_result( result );
    return data;
}

int main() {
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    string dbName   = "localhost";
    string db       = "mysql";
    string user     = "root";
    string passwd   = "PASSWORD"; /* set me first */
    
    string cfgFile = "/etc/mqtt/bridge.json";
    // 
    // First check a few things.
    // 
    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;
        exit(2);
    }
    
    ifstream cfgStream( cfgFile );
    
    config = json::parse(cfgStream);
    
    dbName = config["database"]["name"];
    db     = config["database"]["db"];
    user   = config["database"]["user"];
    passwd = config["database"]["passwd"];
    
    
    conn = mysql_init(NULL);
    
    /* Connect to database */
    if (!mysql_real_connect(conn, dbName.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    
    map<string, string>retData =  getFromMqttQuery("MOTOR"); 
    
    cout << "=== " << retData["name"] << endl;
    /*
     *    // send SQL query 
     *    if (mysql_query(conn, "show tables")) {
     *        fprintf(stderr, "%s\n", mysql_error(conn));
     *        exit(1);
}

res = mysql_use_result(conn);

// output table name
printf("MySQL Tables in mysql database:\n");
while ((row = mysql_fetch_row(res)) != NULL)
    printf("%s \n", row[0]);

// close connection 
mysql_free_result(res);
*/
    mysql_close(conn);
}
