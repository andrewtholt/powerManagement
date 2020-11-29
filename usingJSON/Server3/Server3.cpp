/*
   C socket server example, handles multiple clients using threads
   Compile
   gcc server.c -lpthread -o server
   */

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <fstream>
#include <map>

#include <iostream>

#include "Server3.h"
#include "haRest.h"
using namespace std;

//
// The thread function
//
void *connection_handler(void *);

haRest *connection = nullptr;

map<string,string> cache;
struct globalDefinitions global;

void usage() {
    printf("Usage: Server3 -h| -v -i <HA address> -p <port> -c <cache file>\n");
    exit(2);
}

void dumpCache() {
    for(auto & [n,u]: cache) {
        cout << "Name:" << n << "\tURL:" << u << endl;
    }
}

int main(int argc , char *argv[]) {
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;

    int opt;    

    string cacheFileName("cache.txt");
    global.verbose = false;
    global.debug   = false;
    global.verbose = false;
    global.haIP = "127.0.0.1";
    global.haPort = 8123;
    global.redis = false;
    global.myPort = 9292;

    bool stateOnly = true;

    while ((opt = getopt(argc, argv, "di:c:vhp:")) != -1) {    
        switch(opt) {
            case 'c':
                cacheFileName = optarg;
                break;
            case 'd':
                // return state, complete data from HA
                stateOnly = false;
                break;
            case 'h':    
                usage();    
                break;    
            case 'p':    
                global.myPort = atoi(optarg);    
                break;
            case 'i':
                global.haIP = std::string(optarg);
                break;
            case 'v':
                global.verbose = true;
                break;
        }    
    }

    if( global.verbose ) {
        cout << "Cache Load File :" << cacheFileName <<  endl;
        cout << "HA Server IP    :" << global.haIP << endl;
        cout << "HA Server Port  :" << global.haPort << endl;
        cout << "My Port         :" << global.myPort << endl;
    }

    // instantiate haRest

    connection = new  haRest(global.haIP, global.haPort);

    if(stateOnly) {
        connection->setReturnStateOnly();
    } else {
        connection->clrReturnStateOnly();
    }

    if( global.verbose) {
        connection->setVerbose();
    }
    //
    // Create socket
    //
    socket_desc = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    //    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Bind to localhost
    server.sin_port = htons( global.myPort );
    //
    // Bind
    //
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }

    if(global.verbose) {
        puts("bind done");
    }
    //
    // Load from cache file
    //
    //
    string name, url;
    ifstream cacheData( cacheFileName );

    if(cacheData.is_open()) {

        while (!cacheData.eof()) {
            cacheData >> name >> url;

            cout << name << "-" << url << endl;

            cache[ name ] = url;
        }

        if ( global.verbose ) {
            dumpCache();
        }
    }


    // 
    // Listen
    // 
    listen(socket_desc , 3);
    // 
    // Accept and incoming connection
    // 
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    char buffer[1024];
    int len;
    char *tmp=NULL;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        if(global.verbose) {
            puts("Connection accepted");
        }
        bzero(buffer,sizeof(buffer));

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        if(global.verbose) {
            puts("Handler assigned");
        }
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}
void displayLineHex(uint8_t *a) {
    int i;

    for(i=0;i<16;i++) {
        printf(" %02x",*(a++));
    }
}


void displayLineAscii(uint8_t *a) {
    int i;

    printf(":");

    for(i=0;i<16;i++) {
        if( (*a < 0x20 ) || (*a > 0x80 )) {
            printf(".");
            a++;
        } else {
            printf("%c",*(a++));
        }
    }
    printf("\r\n");
}

void dump(void *ptr, int len ) {
    uint8_t *address = (uint8_t *) ptr;
    int lines=len/16;

    if ( lines == 0) {
        lines=1;
    }

    int i=0;
    for( i = 0; i<len;i+=16) {
        printf("%08lx:", (long unsigned int)address);
        displayLineHex( address );
        displayLineAscii( address );
        address +=16;
    }
}

bool shortCmd(int sock,char *line, char *r) {

    char buffer[1024];

    strcpy(buffer, line);

    bool failFlag = true;

    printf("Short command >%s<\n",buffer);

    std::string cmd((char *)strtok(buffer," \n\r"));

    std::cout << "cmd is " << cmd << std::endl;

    if( cmd.compare("SET") == 0) {
        string key((char *)strtok(NULL," \n\r"));
        cout << "URL is " << cache[key] << endl;

        std::string state((char *)strtok(NULL," \n\r"));

        if (cache.count( key )) {
            failFlag = connection->set(cache[key], state);
            if(failFlag == false) {
                strcpy(r,"OK\n");
            } else {
                strcpy(r,"FAILED\n");
            }
        } else {
            cout << "\tNot Found\n";
        }
    } else if(cmd.compare("GET") == 0) {

        string key((char *)strtok(NULL," \n\r"));
        cout << "URL is " << cache[key] << endl;
        cout << "Key is " << key << endl;

        std::string res =  connection->get(cache[key]) ;
        cout << "Res " << res  << endl;

        res += "\n";

        strcpy(r, res.c_str());

        failFlag = false;
    } else {
        failFlag = true;
        strcpy(r,"FAILED\n");
    }


    return failFlag;
}

/*
 * This will handle connection for each client
 */
void *connection_handler(void *socket_desc) {
    // 
    // Get the socket descriptor
    //
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];

    bool loopFlag = true;
    //
    // Receive a message from client
    //
    char buffer[80];
    int count=0;
    int len=80;

    char reply[255];
    bool failFlag = true;

    while (loopFlag) {
        read_size = recv(sock , client_message , 2000 , 0);

        if( read_size > 0) {
            printf("read_size is %d\n",read_size);

            if(global.verbose) {
                dump( client_message, read_size) ;
            }

            if( isalpha( client_message[0] )) {
                failFlag = shortCmd(sock,client_message, reply);
            } else if (client_message[0] == '*') {
                printf("RESP\n");
            }
            //
            // Send the message back to client
            //
            write(sock , reply , strlen(reply));
            //
            // clear the message buffer
            //
            memset(client_message, 0, 2000);
        } else {
            loopFlag = false;
        }
    }

    if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv failed");
    }

    return 0;
} 

