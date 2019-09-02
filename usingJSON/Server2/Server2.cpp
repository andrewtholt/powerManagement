#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <mysql/mysql.h>

#include "interp.h"
#include "utils.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
json config;

using namespace std;

void *handleConnection(void *);

static int connFd;

struct toThread {
    int newsockfd;
    sockaddr_in* cli_addr;
    string cfgFile;
};

struct connectionFlags {
    bool localHost;     // If true the client has connected to localhost.
    bool quietLocals;   // If true setting a local variable does not reply.
};


void usage(string svcName){
    cout << "Usage: " +svcName + " -h|-c <cfg file> -f -p <port no> -v -l <log file>" << endl;
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

int main(int argc, char* argv[]) {
    int sockfd;
    int portNo=9191;
    int pId;
    int opt;
    bool fg=false;
    bool verbose=false;
    bool iamRoot = false;

    socklen_t len; //store size of the address
    bool loop = false;
    struct sockaddr_in svrAdd, clntAdd;
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

    ifstream cfgStream( cfgFile );
    config = json::parse(cfgStream);

    if( verbose ) {
        cout << "Using config file : " << cfgFile << endl;
    }

    if( getuid() !=0) {
        cerr << "Not root so running in the foreground\n" << endl;
        fg=true;
    }

    if (fg == false) {
        int rc = daemon(true,false);
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

    if( iamRoot) {
        uid_t powerUser = getUserIdByName("power");
        if(setuid(powerUser) != 0) {
            perror("setuid");
            exit(1);
        }
    }

    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0) {
        cerr << "Cannot open socket" << endl;
        return 0;
    }
    //
    // Disables default "wait time" after port is no longer in use before it is unbound.
    //
    int reusePort = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));

    bzero((char*) &svrAdd, sizeof(svrAdd));

    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(portNo);

    //bind socket
    if(bind(sockfd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0) {
        cerr << "Cannot bind" << endl;
        return 0;
    }
    //
    // Number of connections that can be waiting while another finishes
    //
    unsigned int backlogSize = 5; 
    listen(sockfd, backlogSize);

    len = sizeof(clntAdd);

    bool runFlag=true;

    pthread_t thread_id;

    while (runFlag) {
        cout << "Listening" << endl;
        //
        // This is where client connects. svr will hang in this mode until client conn
        // 
        connFd = accept(sockfd, (struct sockaddr *)&clntAdd, &len);

        if (connFd < 0) {
            cerr << "Cannot accept connection" << endl;
        } else {
            cout << "Connection successful" << endl;

            struct toThread *ptr = (struct toThread *) calloc(1, sizeof(struct toThread));
            ptr->newsockfd = connFd;
            ptr->cli_addr = &clntAdd;
            ptr->cfgFile = cfgFile ;

            if(pthread_create(&thread_id, NULL, handleConnection, (void *)ptr) < 0)  {
                perror("could not create thread");
            }
        }

    }
}

void *handleConnection (void *xfer) {
    cout << "Thread No: " << pthread_self() << endl;

    struct toThread *ptr = (struct toThread *)xfer ;
    thread_local struct connectionFlags cFlags;

    cFlags.localHost = false;
    cFlags.quietLocals = false;

    char buffer[255];
    int cmdLen=0;

    bool runFlag = true;
    string request;
    string response;

    vector<string> cmd;
    int len=0;


    char ipAddrBuffer[INET_ADDRSTRLEN];

    int newsockfd;
    sockaddr_in *cli_addr;

    newsockfd = ptr->newsockfd;
    cli_addr = ptr->cli_addr;

    string dbName = config["database"]["name"];
    string db     = config["database"]["db"];
    string user   = config["database"]["user"];
    string passwd = config["database"]["passwd"];

    MYSQL *conn=mysql_init(NULL);

    if (!mysql_real_connect(conn, dbName.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 0, NULL, 0    )) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return(void *) NULL;
    }

    interp myInterp(conn);

    while(runFlag) {    
        bzero(buffer, sizeof(buffer));

        len = read(connFd, buffer, sizeof(buffer));
        if ( len == 0) {
            cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
                << " connection closed by client" << endl;
            runFlag=false;
        } else if ( len < 0) {
            cerr << "ERROR reading from socket" << endl;
        } else if( len > 0 ) {

            request=buffer;
            cmd = split( (trim(request).c_str())) ;
            cmdLen = cmd.size();

            cout << buffer << endl;
            response = myInterp.runCmd( cmd );

            if( response == "CLOSED" ) {
                runFlag = false;
            } else if ( response == "" ) {
                response = "<UNKNOWN>";
            }
            cout << response  << endl;
            response += "\n";

            write(connFd, response.c_str(),response.length());
        }
    }
    cout << "\nClosing thread and conn" << endl;
    mysql_close(conn);
    close(connFd);
    return(void *)NULL;
}
