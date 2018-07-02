#include <stddef.h>  // defines NULL
#include <stdio.h>
#include "myClientClass.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/poll.h>

#include <string.h>

#include <iostream>

// Global static pointer used to ensure a single instance of the class.
myClient* myClient::m_pInstance = NULL;  

/** This function is called to create an instance of the class. 
  Calling the constructor publicly is not allowed. The constructor 
  is private and is only called by this Instance function.
  */

myClient* myClient::Instance() {
//    printf("Instance\n");

    // Only allow one instance of class to be generated.
    if (!m_pInstance) {
        m_pInstance = new myClient;
    } 
    /*
    else {
        printf("Exists\n");
    }
    */

    return m_pInstance;
}

bool myClient::setupNetwork(string hostName, string serviceName)  {
    bool failFlag = true;

    cout << "Host   :" + hostName    << endl;
    cout << "service:" + serviceName << endl;
    return failFlag;
}

bool myClient::setupNetwork(char *hostName, char *serviceName)  {
    bool failFlag = true;

    struct addrinfo *result;
    struct addrinfo *rp;

    if(verbose) {
        printf("Host   :%s\n",hostName);
        printf("Service:%s\n",serviceName);
    }

    int rc = getaddrinfo( hostName, serviceName, NULL, &result);

    failFlag = ( rc !=0 ) ? true : false ;

    for( rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if( sockfd == -1) {
            continue;
        }

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }

        close(sockfd);
    }

    return failFlag;
}

int myClient::getMessage(char *buffer, int32_t len ) {
    int rLen = -1;

    bzero( buffer, len );

    rLen = read(sockfd, buffer, len);

    return rLen;
}

int myClient::sendMessage(char *buffer, int32_t len ) {
    int rLen = -1;

    clientFlush();
    rLen = write(sockfd, buffer, len);

    return rLen;
}

bool myClient::sendCmd(string cmd) {
    bool failFlag = true;
    char buffer[BUFFER_SIZE];

    int rLen=-1;

    clientFlush();
    rLen=sendMessage( (char *)cmd.c_str(), cmd.length() );

    int len=getMessage( buffer, BUFFER_SIZE);

    failFlag = ((strstr(buffer, (char *)"cmd>\n") == NULL)) ? true : false ;

    return failFlag;
}

bool myClient::sendCmd(string cmd, string & res) {
    bool failFlag = true;
    int rLen=-1;
    char buffer[BUFFER_SIZE];

    clientFlush();
    rLen=sendMessage( (char *)cmd.c_str(), cmd.length() );

    int len=getMessage( buffer, BUFFER_SIZE);


    failFlag = ((strstr(buffer, (char *)"cmd>\n") == NULL)) ? true : false ;

    if( failFlag == false ) {
        char *t = buffer;

        strsep(&t,"\n");
        res = buffer;
    }

    return failFlag;
}

bool myClient::loadFile( string fileName ) {
    bool failFlag = true;
    int rLen = -1;

    string cmd = "^load " + fileName + "\n" ;

    if(verbose) {
        cout << "loadFile cmd :" << cmd << endl;
    }

    rLen = sendMessage( (char *)cmd.c_str(), cmd.length());
    failFlag = ( rLen < 0 ) ? true : false ;

    clientFlush();

    return failFlag;
}

bool myClient::clientConnect() {
    bool failFlag = true;
    char buffer[BUFFER_SIZE];
    verbose=true;

    bzero( buffer, BUFFER_SIZE );

    int rLen = -1;

    clientFlush();
    string cmd = "^connect\n" ;

    rLen = sendMessage( (char *)cmd.c_str(), cmd.length());
    failFlag = ( rLen < 0 ) ? true : false ;

    if( failFlag == false) {
        int len=getMessage( buffer, BUFFER_SIZE);

        if (verbose) {
            printf("%s\n",buffer);
        }

        if( len < 0) {
            failFlag = true;
            connected=false;
        } else {
            failFlag = false;
            connected= true;
        }
        /*
        else {
            char *tmp = strstr(buffer,(char *)"CONNECTED");
            if(tmp) {
                char *t = tmp ;

                strsep( &tmp, "\n");

                if(!strcmp(t, "CONNECTED")) {
                    failFlag = false;
                    this->connected = true;
                } else {
                    failFlag = false;
                    this->connected=false;
                }
            }
        }
        */
    }

    return failFlag;
}
bool myClient::clientConnected() {
    return this->connected;
}

void myClient::clientFlush() {
    ssize_t len;
    char buffer[BUFFER_SIZE];
    struct pollfd fds[2];

    bool runFlag=true;

    int ret ;
    do {
        fds[0].fd = sockfd;
        fds[0].events = POLLIN;
        fds[0].revents = 0;

        ret = poll(fds,1, 20 ) ;
        if ( ret > 0 ) {
            len=getMessage( buffer, BUFFER_SIZE);
            runFlag=true;
        } else {
            runFlag = false;
        }
    } while ( runFlag ) ;
}

