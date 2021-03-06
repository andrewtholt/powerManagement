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

#include "utils.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

json config;
using namespace std;
bool verbose=false;
time_t startTime = 0;
uint clientCount=0;

mqd_t toDispatcher = 0;
fstream toLog;

struct toThread {
    int newsockfd;
    sockaddr_in* cli_addr;
    string cfgFile;
};

struct ioDetail {
    string ioType;
    string direction;
};

struct connectionFlags {
    bool localHost;     // If true the client has connected to localhost.
    bool quietLocals;   // If true setting a local variable does not reply.
};

map<string,string> getFromInternalQuery(MYSQL *conn, string name) {
    map<string,string> data;
    
    MYSQL_FIELD *field;
    string sqlCmd = "select * from internalQuery where name='" + name + "';";
    
    if( verbose ) {
        cout << sqlCmd << endl;
    }
    
    int rc = mysql_query(conn, sqlCmd.c_str());
    
    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned int num_fields = mysql_num_fields(result);
    unsigned int num_rows   = mysql_num_rows(result);
    
    if( num_rows > 0 ) {
        char *headers[num_fields];
        for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
            if(verbose) {
                cout << field->name ;
                cout << ":" << row[i] << endl;
            }
            
            data[ field->name ] = row[i];
        }
    }
    return data;
}

map<string,string> getFromSnmpQuery(MYSQL *conn, string name) {
    map<string,string> data;
    
    MYSQL_FIELD *field;
    string sqlCmd = "select * from snmpQuery where name='" + name + "';";
    
    if(verbose) {
        cout << sqlCmd << endl;
    }
    
    int rc = mysql_query(conn, sqlCmd.c_str());
    
    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned int num_rows   = mysql_num_rows(result);
    
    unsigned int num_fields = mysql_num_fields(result);
    
    if( num_rows > 0 ) {
        char *headers[num_fields];
        for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
            if(verbose) {
                cout << field->name ;
                cout << ":" << row[i] << endl;
            }
            
            data[ field->name ] = row[i];
            
        }
    }
    
    mysql_free_result( result );
    return data;
}

map<string,string> getFromMqttQuery(MYSQL *conn, string name) {
    // void getFromMqttQuery(string name) {
    map<string,string> data;
    
    MYSQL_FIELD *field;
    string sqlCmd = "select * from mqttQuery where name='" + name + "';";
    
    if(verbose) {
        cout << sqlCmd << endl;
    }
    
    int rc = mysql_query(conn, sqlCmd.c_str());
    
    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned int num_rows   = mysql_num_rows(result);
    
    unsigned int num_fields = mysql_num_fields(result);
    
    if( num_rows > 0 ) {
        char *headers[num_fields];
        for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
            if(verbose) {
                cout << field->name ;
                cout << ":" << row[i] << endl;
            }
            
            data[ field->name ] = row[i];
            
        }
    }
    
    mysql_free_result( result );
    return data;
}

map<string,string> getFromIoPoint(MYSQL *conn, string name) {
    // void getFromMqttQuery(string name) {
    map<string,string> data;
    
    MYSQL_FIELD *field;
    string sqlCmd = "select * from io_point where name='" + name + "';";
    
    if(verbose) {
        cout << sqlCmd << endl;
    }
    
    int rc = mysql_query(conn, sqlCmd.c_str());
    
    MYSQL_RES *result = mysql_store_result( conn );
    MYSQL_ROW row = mysql_fetch_row(result);
    
    unsigned int num_fields = mysql_num_fields(result);
    unsigned int num_rows   = mysql_num_rows(result);
    //    cout << rc << endl;
    
    if( num_rows  > 0 ) {
        char *headers[num_fields];
        for(unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
            if(verbose) {
                cout << field->name ;
                cout << ":" << row[i] << endl;
            }
            
            data[ field->name ] = row[i];
            
        }
    }
    mysql_free_result( result );
    return data;
}

void snmpPublish(map<string,string> in) {
    struct mq_attr attr;
    string jsonOut;
    
    string outState;
    
    char outBuffer[255];
    bzero(outBuffer,255);
    
    cout << "SNMP Publish" << endl;
    
    jsonOut = "{ ";
    
    jsonOut += "\"type\" : \"snmp\","  ;
    
    jsonOut += "\"ip\" : \"" + in["ipAddress"] + "\",";
    jsonOut += "\"port\" : \"" + in["port"] + "\",";
    
    jsonOut += "\"ro_community\" : \"" + in["ro_community"] + "\",";
    jsonOut += "\"rw_community\" : \"" + in["rw_community"] + "\",";
    
    jsonOut += "\"oid\" : \"" + in["oid"] + "\",";
    
    if ( in["state"] == "ON" ) {
        outState = in["on_value"];
    } else if ( in["state"] == "OFF" ) {
        outState = in["off_value"];
    } else {
        outState = "0";
    }
    
    jsonOut += "\"state\" : \"" + outState + "\"";
    
    jsonOut += " }";
    
    cout << jsonOut << endl;
    
    if( toDispatcher == 0) {
        toDispatcher=mq_open("/toDispatcher", O_WRONLY|O_CREAT,0664, &attr);
        if( toDispatcher == -1) {
            perror("mq_open");
            exit(2);
        }
    }
    
    strncpy(outBuffer, jsonOut.c_str(), jsonOut.length());
    
    if (mq_send(toDispatcher, outBuffer, strlen(outBuffer), 0) < 0) {
        perror("mq_send");
    }
    
}

void mqttPublish(string topic, string msg) {
    struct mq_attr attr;
    string jsonOut;
    
    memset(&attr, 0, sizeof attr);
    //max size of a message
    attr.mq_msgsize = 255;  //MSG_SIZE = 4096
    attr.mq_flags = 0;
    //maximum of messages on queue
    attr.mq_maxmsg = 10;
    
    struct pollfd fds[2];
    
    cout << "MQTT Publish:" << endl;
    
    if( toDispatcher == 0) {
        toDispatcher=mq_open("/toDispatcher", O_WRONLY|O_CREAT,0664, &attr);
        if( toDispatcher == -1) {
            perror("mq_open");
            exit(2);
        }
    }
    
    jsonOut = "{ \"type\" : \"mqtt\",";
    jsonOut += "\"topic\" : \"" + topic + "\",";
    jsonOut += "\"state\" : \"" + msg+ "\" }\n";
    
    cout << jsonOut << endl;
    
    time_t now ;
    struct tm tmNow;
    char timeBuffer[255];
    
    if ( toLog ) {
        now = time(NULL);
        tmNow = *localtime(&now);
        
        sprintf(timeBuffer, "%d:%d:%d ", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
        
        toLog << timeBuffer << " " ;
        toLog << jsonOut << endl << flush;
    }
    
    if (mq_send(toDispatcher, jsonOut.c_str(), jsonOut.length(), 0) < 0) {
        perror("mq_send");
    }
}

//
// Set old=new for given value
//
void dbReset(MYSQL *conn, string name) {
    map<string, string>row;
    
    row=getFromIoPoint(conn, name);
    transform((row["io_type"]).begin(), (row["io_type"]).end(), (row["io_type"]).begin(), ::tolower);
    string dataType = row["io_type"];
    
    //    string sqlCmd = "update " + dataType + " set old_state = state where name='" + name + "';";
    string sqlCmd = "update io_point set old_state = state where name='" + name + "';";
    int rc = mysql_query(conn, sqlCmd.c_str());
}
// 
// Set old=new for all values
//
void dbReset(MYSQL *conn) {
    int rc;
    string sqlCmd;
    
    sqlCmd = "update io_point set old_state = state;";
    rc = mysql_query(conn, sqlCmd.c_str());
    
    /*
     *       sqlCmd = "update modbus set old_state = state;";
     *       rc = mysql_query(conn, sqlCmd.c_str());
     * 
     *       sqlCmd = "update mqtt set old_state = state;";
     *       rc = mysql_query(conn, sqlCmd.c_str());
     * 
     *       sqlCmd = "update snmp set old_state = state;";
     *       rc = mysql_query(conn, sqlCmd.c_str());
     */
    
}

void updateIO(MYSQL *conn, map<string, string>row) {
    
    string name = row["name"];
    string state = row["state"];
    
    string sqlCmd;
    
    transform((row["io_type"]).begin(), (row["io_type"]).end(), (row["io_type"]).begin(), ::tolower);
    
    //    sqlCmd = "update "+ row["io_type"] +" set old_state=state, state = '" + row["state"] + "' where name='" + name + "';";
    sqlCmd = "update io_point set old_state=state, state = '" + row["state"] + "' where name='" + name + "';";
    
    int rc = mysql_query(conn, sqlCmd.c_str());
    
    if(verbose) {
        cout << "updateIO:" << rc << endl;
        cout << sqlCmd << endl;
    }
    
    if( row["io_type"] == "mqtt" ) {
        map<string,string> mqttQuery = getFromMqttQuery(conn, name) ;
        
        string topic = mqttQuery["topic"];
        //        string state = mqttQuery["state"];
        string oldState = mqttQuery["old_state"];
        
        if( state != oldState ) {
            if( row["direction"] == "OUT" ) {
                mqttPublish( topic, state) ;
            }
            sqlCmd = "update mqtt set old_state=state where name='" + name + "';";
            cout << sqlCmd << endl;
            rc = mysql_query(conn, sqlCmd.c_str());
        }
    } else if( row["io_type"] == "snmp" ) {
        map<string,string> snmpQuery = getFromSnmpQuery(conn, name) ;
        string oldState = snmpQuery["old_state"];
        if( state != oldState ) {
            if( row["direction"] == "OUT" ) {
                snmpPublish(snmpQuery);
            }
            sqlCmd = "update snmp set old_state=state where name='" + name + "';";
            cout << sqlCmd << endl;
            rc = mysql_query(conn, sqlCmd.c_str());
        }
    }
    
    /*
     *       string sqlCmd = "update "+ row["io_type"] +" set old_state=state, state = '" + row["state"] + "' where name='" + name + "';";
     * 
     *       int rc = mysql_query(conn, sqlCmd.c_str());
     * 
     *       cout << "updateIO:" << rc << endl;
     *       cout << sqlCmd << endl;
     */
    
}

void coldStart(MYSQL *conn) {
    
    if(clientCount == 1) {
        string name;
        string sql = "select name from io_point where state = 'OFF' or state = 'ON';";
        
        mysql_query(conn, sql.c_str());
        
        MYSQL_RES *confres = mysql_store_result(conn);
        int totalrows = mysql_num_rows(confres);
        int numfields = mysql_num_fields(confres);
        MYSQL_FIELD *mfield;
        MYSQL_ROW row;
        
        while((row = mysql_fetch_row(confres))) {
            
            cout << row[0] << endl;
            name = row[0];
            
            map<string,string> dataRow=getFromIoPoint(conn, name);
            
            dataRow["state"] = "OFF";
            dataRow["old_state"] = "OFF";
            
            updateIO(conn, dataRow);
        }
    }
}

vector<string> handleRequest(MYSQL *conn, string request, struct connectionFlags *cFlags, map<string,string> &localVariable) {
    vector<string> response;
    vector<string> cmd;
    
    map<string,ioDetail> cache ;
    map<string, string>row;
    
    //    thread_local map<string, string> localVariable;
    
    string name;
    string value;
    
    static uint64_t uptime=0;
    
    bool validCmd = false;
    time_t now = time(NULL);
    
    uptime = now - startTime;
    
    localVariable["$UPTIME"] = to_string( uptime );
    localVariable["$CLIENTS"] = to_string( clientCount );
    
    cmd = split( (trim(request).c_str())) ;
    int cmdLen = cmd.size();
    
    cout << ">" + cmd[0]  + "<" << endl;
    
    validCmd = false;
    switch( cmdLen ) {
        case 1:
            // A small measure of security.
            // Global commands can only be executed from the same host.
            //
            if( cFlags->localHost ) {
                if( cmd[0] == "COLD" ) {
                    validCmd = true;
                    coldStart(conn);
                    response.push_back(string("COLD\n")); // Respond with "Pong!" 
                }
            } 
            if( cmd[0] == "PING" ) {
                validCmd = true;
                response.push_back(string("Pong!\n")); // Respond with "Pong!" 
            } else if( cmd[0] == "CLOSE" ) {
                validCmd = true;
                response.push_back(string("CLOSED\n")); 
            } else if(cmd[0] == "RESET" ) {
                validCmd = true;
                dbReset(conn);
                response.push_back(string("RESET\n")); 
            } else if (cmd[0] == "SILENT") {
                validCmd = true;
                cFlags->quietLocals = true;
            } else if (cmd[0] == "VERBOSE") {
                validCmd = true;
                cFlags->quietLocals = false;
            }
            break;
        case 2:
            if( cmd[0] == "RESET" ) {
                validCmd = true;
                dbReset(conn, cmd[1]);
                response.push_back(string("RESET\n")); 
            } else if( cmd[0] == "TOGGLE" ) {
                string out;
                validCmd = true;
                row=getFromIoPoint(conn, cmd[1]);
                
                if( row.size() > 0) {
                    if( row["state"] == "ON" ) {
                        row["state"] = "OFF";
                    } else {
                        row["state"] = "ON";
                    }
                    out = row["state"];
                } else {
                    out = "<UNDEFINED>";
                }
                updateIO(conn, row);
                response.push_back(out +"\n");
                
                /* 
                 *                if( row.size() == 0) {
                 *                    response.push_back( "<UNDEFINED>\n");
            } else {
                transform((row["io_type"]).begin(), (row["io_type"]).end(), (row["io_type"]).begin(), ::tolower);
                
                string ioType = row["io_type"] ;
                
                if( ioType == "mqtt" ) {
                    cout << "I'm MQTT" << endl;
                    row = getFromMqttQuery(conn, cmd[1]) ;
                    row["io_type"] = ioType;
            }
            if( ioType == "snmp" ) {
                cout << "I'm SNMP" << endl;
                row = getFromSnmpQuery(conn, cmd[1]) ;
            }
            
            if( row.size() > 0 ) {
                if( row["state"] == "ON" ) {
                    row["state"] = "OFF";
            } else {
                row["state"] = "ON";
            }
            updateIO(conn, row);
            response.push_back( row["state"]+ "\n");
            }
            }
            */
                
            } else if( cmd[0] == "GET" ) {
                string out;
                string value;
                
                validCmd = true;
                cout << "GET " << cmd[1] << endl;
                
                if( cmd[1].at(0) == '$' ) {
                    cout << "Local " << endl;
                    
                    string tmp = cmd[1];
                    
                    if ( localVariable[ tmp ] == "" ) {
                        value = "<UNDEFIND>";
                    } else {
                        value = localVariable[ tmp ];
                    }
                } else {
                    row=getFromIoPoint(conn, cmd[1]);
                    if(  row.size() == 0) {
                        out = "<UNDEFINED>";
                    } else {
                        value = row["state"];
                    }
                }
                
                if ( localVariable["$PROTOCOL"] == "JSON" ) {
//                    out = "{\"name\":\"" + cmd[1] + "\",\"value\":\"" + row["state"] + "\"}";
                    out = "{\"name\":\"" + cmd[1] + "\",\"value\":\"" + value + "\"}";
                } else {
                    out = value;
                    /*
                    if(  row.size() == 0) {
                        out = "<UNDEFINED>";
                    } else {
                        out = value;
                    }
                    */
                }
                response.push_back(out + "\n");
            }
            break;
            case 3:
                if( cmd[0] == "SET" ) {
                    validCmd = true;
                    cout << "SET " << cmd[1] + " to " + cmd[2] << endl;
                    string out;
                    
                    if( cmd[1].at(0) == '$') {
                        string tmp = cmd[1];
                        localVariable[tmp ] = cmd[2];
                        if( cFlags->quietLocals) {
                            cout << "More SSSSHHHH" << endl;
                            out="";
                        } else {
                            out = cmd[2];
                        }
                    } else {
                        row=getFromIoPoint(conn, cmd[1]);
                        
                        if( row.size() > 0 ) {
                            row["state"] = cmd[2];
                            updateIO(conn, row);
                            out = string(row["state"]);
                        } else {
                            out=string("<UNDEFINED>"); 
                        }
                    }
                    
                    if( out.length() > 0) {
                        if ( localVariable["$PROTOCOL"] == "JSON" ) {
                            cout << "JSON" << endl;
                            //                    out = "{\"" + cmd[1] + "\":\"" + cmd[2] + "\"}";
                            out = "{\"name\":\"" + cmd[1] + "\",\"value\":\"" + cmd[2] + "\"}";
                        }
                        response.push_back(out +"\n"); 
                    }
                }
                break;
    }
    
    if (validCmd == false ) {
        response.push_back(string("<UNIMPLEMENTED_CMD>\n")); 
    }
    
    return response;
}

void *handleConnection(void *xfer) {
    struct toThread *ptr = (struct toThread *)xfer ;
    
    //    bool localHost = false;
    //
    thread_local struct connectionFlags cFlags;
    
    cFlags.localHost = false;
    cFlags.quietLocals = false;
    
    char ipAddrBuffer[INET_ADDRSTRLEN];
    
    int newsockfd; 
    sockaddr_in *cli_addr;
    
    newsockfd = ptr->newsockfd;
    cli_addr = ptr->cli_addr;
    
    struct in_addr ipAddr= cli_addr->sin_addr;
    inet_ntop( AF_INET, &ipAddr, ipAddrBuffer,  INET_ADDRSTRLEN);
    
    cFlags.localHost = (!strcmp(ipAddrBuffer,"127.0.0.1")) ? true : false ;
    
    char buffer[256]; // Initialize buffer to zeros
    bzero(buffer, 256);
    MYSQL *conn;
    
    string dbName = config["database"]["name"];
    string db     = config["database"]["db"];
    string user   = config["database"]["user"];
    string passwd = config["database"]["passwd"];
    
    conn=mysql_init(NULL);
    
    /* Connect to database */
    if (!mysql_real_connect(conn, dbName.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return (void*)NULL;
    }
    
    bool runFlag = true ;
    
    char *token=NULL;
    char *rest ;
    
    clientCount++;
    thread_local map<string, string> localVariable;
    localVariable["$TEST"] = "DEFAULT";
    localVariable["$PROTOCOL"] = "DEFAULT";  // Set protocol.
    
    while (runFlag) {
        
        bzero(buffer, sizeof(buffer));
        int n = read(newsockfd, buffer, 255);
        if (n == 0) {
            cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
            << " connection closed by client" << endl;
            runFlag=false;
        } else if (n < 0) {
            cerr << "ERROR reading from socket" << endl;
        } else if( n > 0 ) {
            rest = buffer;
            token = strtok_r(rest, "\r\n", &rest);
            if( token != NULL) {
                printf("%s\n", token);
                vector<string> response = handleRequest(conn, token, &cFlags, localVariable); // Get the response
                
                for (int i = 0; i < response.size(); i++) {
                    cout << i << ":" << response[i] << endl;
                    n = write(newsockfd, response[i].c_str(), response[i].length()); // Write response by line
                    if (n < 0) {
                        cerr << "ERROR writing to socket" << endl;
                    }
                    if (response[i] == "CLOSED\n") {
                        close(newsockfd); // Close the connection if response line == "CLOSE"
                        cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
                        << " connection terminated" << endl;
                        runFlag = false;
                    } 
                }
            }
        }
    }
    
    if( clientCount > 0) {
        clientCount--;
    }
    cout << "Bye" << endl;
    return (void *)NULL;
}

void usage(string svcName){
    cout << "Usage: Server -h|-c <cfg file> -f -p <port no> -v -l <log file>" << endl;
    cout << endl;
    
    cout << "\t-h\t\tHelp." << endl;
    cout << "\t-c <cfg>\tUse specified config file." << endl;
    cout << "\t-f\t\tRun in the foreground." << endl;
    cout << "\t-p <port no>\tListen on this port." << endl;
    cout << "\t-v\t\tVerbose." << endl;
    cout << "\t-l <file name>\tLog to this file." << endl;
    cout << endl;
    
    cout << "\tDefault is equivalent to:" << endl;
    cout << "\t\t Server -c /etc/mqtt/bridge.json -p 9191" << endl << endl ;
}


int main(int argc,  char *argv[]) {
    int sockfd; // Socket file descriptor
    int portNo=9191; // Port number
    int opt;
    bool fg=false;
    verbose=false;
    
    sockaddr_in serv_addr; // Server address
    string svcName = basename(argv[0]) ;
    string cfgFile = "/etc/mqtt/bridge.json";
    string logFile = "";
    
    while( (opt = getopt(argc, argv, "c:fhp:vl:")) != -1) {
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
            case 'p':
                portNo = atoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case 'l':
                logFile = optarg;
                break;
        }
    }
    bool iamRoot=false ;
    if( getuid() !=0) {
        cerr << "Not root so running in the foreground\n" << endl;
        fg=true;
        
        iamRoot = true;
        //        verbose = true;
    }
    
    ifstream cfgStream( cfgFile );
    config = json::parse(cfgStream);
    
    if( verbose ) {
        cout << "Using config file : " << cfgFile << endl;
    }
    
    int rc=0;
    
    if (fg == false) {
        rc = daemon(true,false);
        //        rc = daemon(true,true);
        
        string pidFile = "/var/run/" + svcName + ".pid";
        
        FILE *run=fopen(pidFile.c_str(), "w");
        
        if( run == NULL) {
            perror("PID File");
            exit(2);
        }
        
        fprintf(run,"%d\n", getpid());
        
        fclose(run);
        run=NULL;
    }
    
    if( logFile != "") {
        toLog.open(logFile,ios::out);
        if( !toLog ) {
            cerr << "Failed to create "+ logFile << endl;
        }
    }
    
    uid_t powerUser = getUserIdByName("power");
    
    if( iamRoot == false) {
        if(setuid(powerUser) != 0) {
            perror("setuid");
            exit(1);
        }
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create new socket, save file descriptor
    if (sockfd < 0) {
        cerr << "ERROR opening socket" << endl;
    }
    
    int reusePort = 1; // Disables default "wait time" after port is no longer in use before it is unbound
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
    
    bzero((char *) &serv_addr, sizeof(serv_addr)); // Initialize serv_addr to zeros
    //    portno = atoi(argv[1]); // Reads port number from char* array
    
    serv_addr.sin_family = AF_INET; // Sets the address family
    serv_addr.sin_port = htons(portNo); // Converts number from host byte order to network byte order
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Sets the IP address of the machine on which this server is running
    
    if (bind(sockfd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // Bind the socket to the address
        cerr << "ERROR on binding" << endl;
    
    unsigned int backlogSize = 5; // Number of connections that can be waiting while another finishes
    listen(sockfd, backlogSize);
    cout << "C++ server opened on port " << portNo << endl;;
    
    pthread_t thread_id;
    
    int newsockfd; // New socket file descriptor
    unsigned int clilen; // Client address size
    sockaddr_in cli_addr; // Client address
    
    startTime = time(NULL);
    while (true) {
        clilen = sizeof(sockaddr_in);
        newsockfd = accept(sockfd, (sockaddr *) &cli_addr, &clilen); // Block until a client connects
        if (newsockfd < 0)
            cerr << "ERROR on accept" << endl;
        
        cout << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port)
        << " connected" << endl;
        
        struct toThread *ptr = (struct toThread *) calloc(1, sizeof(struct toThread));
        
        ptr->newsockfd = newsockfd;
        ptr->cli_addr = &cli_addr;
        //        ptr->cfgFile = "/etc/mqtt/bridge.json";
        ptr->cfgFile = cfgFile ;
        
        if( pthread_create( &thread_id , NULL ,  handleConnection , (void *)ptr) < 0) {
            perror("could not create thread");
            return 1;
        }
    }
    return 0;
}
