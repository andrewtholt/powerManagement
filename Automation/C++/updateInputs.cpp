#include "myClientClass.h"
#include <string.h>
#include <unistd.h>
#include <iostream>


using namespace std;

void usage() {
    printf("Usage\n");
}

int main(int argc, char *argv[]) {
    bool verbose=false;

    int len=0;
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];

    string hostName = "localhost";
    string serviceName ="myclient" ;

    int opt;

    while (( opt = getopt(argc, argv, "h?n:p:v")) !=-1) {
        switch(opt) {
            case '?':
            case 'h':
                usage();
                exit(0);
            case 'n':
                hostName = optarg;
                break;
            case 'p':
                serviceName = optarg;
                break;
            case 'v':
                verbose = true;
                break;
        }
    }

    if(verbose) {
        cout << "Hostname : " << hostName << endl;
        cout << "Service  : " << serviceName << endl;
    }

    myClient *n = myClient::Instance();

    bool failFlag = n->setupNetwork((char *)hostName.c_str(), (char *)serviceName.c_str());

    if(verbose) {
        printf("Network setup %s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }

    if(failFlag) {
        exit(1);
    }

    failFlag = n->loadFile( "mysqlCmds.txt");

    if(verbose) {
        cout << "loadFile ";
        printf("%s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }

    failFlag = n->clientConnect() ;

    if(verbose) {
        cout << "clientConnect ";
        printf("%s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }

    if ( n->clientConnected() == false ) {
        fprintf(stderr, "FATAL ERROR: Connected to interface but not to database\n");
        exit(2);
    }

    string out;
    string sql = "select topic from io_point where direction = 'in';\n";
    len = n->sendCmd( sql );

    string cmd = "^get-row-count\n";
    len = n->sendCmd( cmd, out );

    int cnt = stoi( out,nullptr);

    cout << cnt << endl;

    for( int i=0; i < cnt ; i++ ) {
        if( i == 0 ) {
            cmd = "^get-col topic\n";
        } else {
            cmd = "^go-next\n";
            len = n->sendCmd( cmd );
            cmd = "^get-col topic\n";
        }
        len = n->sendCmd( cmd, out );
        cout << out << endl;
    }
}

