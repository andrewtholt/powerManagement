/*
 * notify.cpp
 *
 *  Created on: 20 Jun 2019
 *      Author: andrewh
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <mysql/mysql.h>

#include <sys/types.h>
#include <pwd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mqueue.h>
#include <poll.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <thread>
#include <set>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

map<string,string> getToMap(MYSQL *conn, string sqlCmd) {
	map<string,string> data;

	MYSQL_FIELD *field;

//	cout << sqlCmd << endl;

	int rc = mysql_query(conn, sqlCmd.c_str());

	MYSQL_RES *result = mysql_store_result( conn );
	MYSQL_ROW row = mysql_fetch_row(result);
	unsigned int num_fields = mysql_num_fields(result);
	unsigned int num_rows   = mysql_num_rows(result);

	if( num_rows > 0 ) {
		char *headers[num_fields];
		for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
//			cout << field->name ;
//			cout << ":" << row[i] << endl;

			data[ field->name ] = row[i];
		}
	}
	return data;
}

struct timestamp {
	long old;
	long stamp;
};
int main() {
	MYSQL *conn;
	map<string, string>result;

	map<string, struct timestamp>cache;
	bool firstTime=true;

	string cfgFile = "/etc/mqtt/bridge.json";

	ifstream cfgStream( cfgFile );
	json config = json::parse(cfgStream);

	string dbName = config["database"]["name"];
	string db     = config["database"]["db"];
	string user   = config["database"]["user"];
	string passwd = config["database"]["passwd"];

	conn=mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, dbName.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return 1;
	}


	cache["mqtt"].old = 0;
	cache["mqtt"].stamp = 0;

	cache["internal"].old = 0;
	cache["internal"].stamp = 0;

	string sqlCmd;

	//	long now = (unsigned)time(NULL);
	long old=0;
	set<string> changed;
	long stamp;

	while(true) {
		for (pair<string, struct timestamp> element : cache) {
			string word = element.first;
			// Accessing VALUE from element.
			long count = element.second.old;
			cout << word << " :: " << count << endl;

			sqlCmd = "select name,state,unix_timestamp(logtime) from " + word + " order by logtime desc limit 1;";
			cout << sqlCmd << endl;

			result = getToMap(conn, sqlCmd) ;

			stamp = atol((result["unix_timestamp(logtime)"]).c_str());
			cache[word].stamp = stamp;

			if( stamp > cache[word].old ) {
				cout << "====== Check " + word << endl;
				sqlCmd="select name,state,unix_timestamp(logtime) from mqtt where logtime > from_unixtime(" + to_string(cache[word].old) + ");";
				cout << sqlCmd << endl;
				cache[word].old = stamp;
			}

		}
		sleep(10);
	}
	return 0;
}
