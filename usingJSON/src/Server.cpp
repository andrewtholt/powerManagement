#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <sys/poll.h>

#include <my_global.h>
#include <mysql.h>

#include <thread>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

json config ;
bool verbose=false;

struct ioDetail {
    string ioType;
    string direction;
};

mqd_t toDispatcher = 0;

map<string,ioDetail> cache ;
/*
Needs:

sudo apt-get install libmysqlclient-dev
*/
vector<string> split(const char *str, char c = ' ') {
    vector<string> result;

    do {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}

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

void finish_with_error(MYSQL *con) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    //    exit(1);
}

struct ioDetail typeFromCache(MYSQL *con, string name) {
    string sqlCmd;
    string ioType;
    struct ioDetail io;

    int cacheCount =  cache.count(name);
    if( cacheCount == 0) {
        // 
        // Not in the cache so go and get it.
        //
        sqlCmd = "select io_type,direction from io_point where name='" + name + "';";
        if(verbose ) {
            cout << sqlCmd << endl;
        }
        if( mysql_query(con, sqlCmd.c_str())) {
            cerr << "SQL Error" << endl;
            ioType = "<UNDEFINED>";
        } else {
            MYSQL_RES *result = mysql_store_result(con);
            MYSQL_ROW row = mysql_fetch_row(result);

            io.ioType = row[0];
            io.direction = row[1];

            cache[name] = io;
            ioType = row[0];
            mysql_free_result(result);
        }
    } else {
        io = cache[name];
    }

    transform((io.ioType).begin(), (io.ioType).end(), (io.ioType).begin(), ::tolower);

    return io;
}

void mqttPublish(MYSQL *con, string name) {
    string sqlCmd;
    string state;
    string jsonOut;
    struct mq_attr attr;

    memset(&attr, 0, sizeof attr);
    //max size of a message
    attr.mq_msgsize = 255;  //MSG_SIZE = 4096
    attr.mq_flags = 0;
    //maximum of messages on queue
    attr.mq_maxmsg = 10;

    struct pollfd fds[2];

    cout << "Publish:" << endl;

    if( toDispatcher == 0) {
        toDispatcher=mq_open("/toDispatcher", O_WRONLY|O_CREAT,0664, &attr);
        if( toDispatcher == -1) {
            perror("mq_open");
            exit(2);
        }
    }

    struct ioDetail io;
    io = typeFromCache(con, name) ;

    string ioType = io.ioType;
    string dir = io.direction;

    if( verbose ) {
        cout << "Name      :" + name << endl;
        cout << "IO Type   :" + ioType << endl;
        cout << "Direction :" + dir  << endl;
    }

    if( ioType == "mqtt" ) {
        sqlCmd = "select topic,state from mqttQuery where name = '" + name + "';";

        if(verbose ) {
            cout << sqlCmd << endl;
        }
        if( mysql_query(con, sqlCmd.c_str())) {
            cerr << "SQL Error" << endl;
            ioType = "<UNDEFINED>";
        } else {
            string topic;
            MYSQL_RES *result = mysql_store_result(con);
            MYSQL_ROW row = mysql_fetch_row(result);

            topic = row[0];
            state = row[1];
            if( verbose ) {
                cout << "Topic     :" + topic << endl;
                cout << "State     :" + state << endl;
            }

            jsonOut = "{ \"type\" : \"" + ioType + "\",";
            jsonOut += "\"topic\" : \"" + topic + "\",";
            jsonOut += "\"state\" : \"" + state + "\" }\n";

            cout << jsonOut << endl;

            fds[1].fd = toDispatcher;
            fds[1].events = POLLOUT;

            int ret = poll(fds, 2, 0);

            if (fds[1].revents & POLLOUT) {
                if (mq_send(toDispatcher, jsonOut.c_str(), jsonOut.length(), 0) < 0) {
                    perror("mq_send");
                }
            }
        }
    }
}

bool updateIO(string name, string value) {
    bool failFlag=true;

    struct ioDetail io;

    string dbName = config["database"]["name"];
    string db     = config["database"]["db"];
    string user   = config["database"]["user"];
    string passwd = config["database"]["passwd"];

    string sqlCmd ;

    MYSQL *con=mysql_init(NULL);

    if ( con == NULL ) {
        cerr << "DB Connect failed" << endl;
        return true;
    }
    io = typeFromCache(con, name) ;

    if( io.ioType == "<undefined>") {
        failFlag=true;
    } else {

        if (mysql_real_connect(con, dbName.c_str(), "automation", "automation", "automation", 0, NULL, 0) == NULL) {
            finish_with_error(con);
            return(true);
        }

        //        sqlCmd = "update "+ io.ioType+" set old_state=state, state = '" + value + "' where name='" + name + "' and old_state <> '" + value + "';";
        sqlCmd = "update "+ io.ioType+" set old_state=state, state = '" + value + "' where name='" + name + "';";

        if(verbose) {
            cout << sqlCmd << endl;
        }
        if( mysql_query(con, sqlCmd.c_str())) {
            cerr << "SQL Error" << endl;
            cerr << sqlCmd << endl;
        } else {
            if( mysql_affected_rows(con) > 0) {
                if( io.ioType == "mqtt") {
                    mqttPublish( con, name );
                }
                // 
                // Database update.
                //
                // I know what the io is, what direction.
                // So if it's:
                // MQTT OUT and it's changed something needs to publish.
                //
            }
            failFlag=false;
        }
    }
    mysql_close( con );
    return failFlag;
}

vector<string> handleRequest(string request) {
    vector<string> response;
    vector<string> stuff ;
    int cmdLen=0;

    bool validCmd = false;

    string cmd = rtrim(request) ;

    stuff = split(cmd.c_str());
    cmdLen = stuff.size();

    string dbName = config["database"]["name"];

    MYSQL *con=mysql_init(NULL);

    if ( con == NULL ) {
        cerr << "DB Connect failed" << endl;
        return response;
    }

    if (mysql_real_connect(con, dbName.c_str(), "automation", "automation", "automation", 0, NULL, 0) == NULL) {
        finish_with_error(con);
        return response;
    }


    switch(cmdLen) {
        case 1:
            if ( stuff[0] == "CLOSE") { 
                response.push_back(string("LOCAL\n")); 
                response.push_back(string("BYE\n")); 
                response.push_back(string("CLOSE")); // Close the connection with poison pill
                validCmd = true;
            }
        case 2:
            if ( stuff[0] == "GET") { 
                string name = stuff[1];
                string sqlCmd;

                struct ioDetail io = typeFromCache(con, name);

                if ( io.ioType == "mqtt" ) {
                    sqlCmd = "select state from mqttQuery where name='" + name + "';";
                } else if (io.ioType == "internal") {
                    sqlCmd = "select state from internalQuery where name='" + name + "';";
                }

                if(verbose) {
                    cout << sqlCmd << endl;
                }

                if( mysql_query(con, sqlCmd.c_str())) {
                    cerr << "SQL Error" << endl;
                } else {
                    MYSQL_RES *result = mysql_store_result(con);
                    MYSQL_ROW row = mysql_fetch_row(result);

                    string value = "<UNDEFINED>";

                    if( row ) {
                        cout << row[0] << endl;
                        value = row[0];
                    }

                    //                    response.push_back(string("SET " + name + " " + value +"\n")); 
                    response.push_back(string(value +"\n")); 
                    mysql_free_result(result);
                }
            } 
            validCmd = true;
            break;
        case 3:
            if( stuff[0] == "SET") {
                string sqlCmd;
                string name = stuff[1];
                string value = stuff[2];
                string ioType="NONE";

                bool ff=updateIO( name, value) ;

                validCmd = true;
            }
            break;
        default:
            validCmd = false;
            break;
    }
    if( validCmd == false) {
        response.push_back(string("Pong!\n")); // Respond with "Pong!" 
    }

    mysql_close( con );
    return response;
}

void handleConnection(int newsockfd, sockaddr_in* cli_addr) {
    char buffer[256]; // Initialize buffer to zeros
    bzero(buffer, 256);

    while (true) {
        int n = read(newsockfd, buffer, 255);
        if (n == 0) {
            cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
                << " connection closed by client" << endl;
            return;
        }
        else if (n < 0)
            cerr << "ERROR reading from socket" << endl;

        stringstream stream;
        stream << buffer << flush;
        while (stream.good()) {
            string request;
            getline(stream, request); // Get and print request by lines
            if (request.length() > 0) {
                cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
                    << ": " << request << endl;

                vector<string> response = handleRequest(request); // Get the response

                for (int i = 0; i < response.size(); i++) {
                    string output = response[i];
                    if (output != "CLOSE") {
                        n = write(newsockfd, output.c_str(), output.length()); // Write response by line
                        if (n < 0)
                            cerr << "ERROR writing to socket" << endl;
                    }
                    else {
                        close(newsockfd); // Close the connection if response line == "CLOSE"
                        cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
                            << " connection terminated" << endl;
                        return;
                    }
                }
            }
        }
    }
}

void usage() {
}

int main(int argc, char *argv[]) {

    int opt;

    int sockfd; // Socket file descriptor
    string cfgFile = "/etc/mqtt/bridge.json";

    sockaddr_in serv_addr; // Server address

    //    string listenOn = "127.0.0.1";
    int portNo = 9191;

    while( (opt = getopt(argc, argv, "c:hi:p:v")) != -1) {
        switch(opt) {
            case 'c':
                cfgFile = optarg;
                break;
            case 'h':
                usage();
                exit(1);
                break;
            case 'p':
                portNo = atoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
        }
    }
    if(verbose) {
        printf("MySQL client version: %s\n", mysql_get_client_info());
    }
    // 
    // First check a few things.
    // 
    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;
        exit(2);
    }

    ifstream cfgStream( cfgFile );
    /*
       string str;
       ostringstream ss;

       ss << cfgStream.rdbuf();

       str = ss.str();
       */
    config = json::parse(cfgStream);

    string dbName = config["database"]["name"];

    MYSQL *con=mysql_init(NULL);

    if ( con == NULL ) {
        cerr << "DB Connect failed" << endl;
        exit(3);
    }

    if (mysql_real_connect(con, dbName.c_str(), "automation", "automation", "automation", 0, NULL, 0) == NULL) {
        finish_with_error(con);
    }

    string sqlCmd = "select name,io_type,direction from io_point;";

    if (mysql_query(con, sqlCmd.c_str()) ) {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {

        if( verbose ) {
            cout << row[0] << endl;
            cout << row[1] << endl;
            cout << row[2] << endl;
            cout << "=========" << endl;
        }

        struct ioDetail io;
        io.ioType = row[1];
        io.direction = row[2];

        cache[ row[0]] = io;
    }


    mysql_close(con);

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create new socket, save file descriptor
    if (sockfd < 0) {
        cerr << "ERROR opening socket" << endl;
    }

    int reusePort = 1; // Disables default "wait time" after port is no longer in use.
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));

    bzero((char *) &serv_addr, sizeof(serv_addr)); // Initialize serv_addr to zeros

    serv_addr.sin_family = AF_INET; // Sets the address family
    serv_addr.sin_port = htons(portNo); 
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Sets the IP address to listen on.

    if (bind(sockfd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // Bind socket to the address
        cerr << "ERROR on binding" << endl;

    unsigned int backlogSize = 5; 
    listen(sockfd, backlogSize);

    if( verbose ) {
        cout << "C++ server opened on port " << portNo << endl;;
    }

    while (true) {
        int newsockfd; // New socket file descriptor
        unsigned int clilen; // Client address size
        sockaddr_in cli_addr; // Client address

        clilen = sizeof(sockaddr_in);
        newsockfd = accept(sockfd, (sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0)
            cerr << "ERROR on accept" << endl;

        cout << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port)
            << " connected" << endl;

        handleConnection(newsockfd, &cli_addr); // Handle the connection
    }
    return 0;
}