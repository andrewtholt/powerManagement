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
#include <thread>

using namespace std;

struct toThread {
    int newsockfd;
    sockaddr_in* cli_addr;
};

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

/*
map<string,string> getFromMqttQuery(string name) {
// void getFromMqttQuery(string name) {
    map<string,string> data;
    
    MYSQL_FIELD *field;
    string sqlCmd = "select * from mqttQuery where name='" + name + "';";
    
    cout << sqlCmd << endl;
    
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
*/

vector<string> handleRequest(string request) {
    vector<string> response;
    vector<string> cmd;
    
    //    string cmd = trim(request);
    cmd = split( (trim(request).c_str())) ;
    int cmdLen = cmd.size();
    
    cout << ">" + cmd[0]  + "<" << endl;
    
    switch( cmdLen ) {
        case 1:
            if( cmd[0] == "PING" ) {
                response.push_back(string("Pong!\n")); // Respond with "Pong!" 
            } else if( cmd[0] == "CLOSE" ) {
                response.push_back(string("CLOSE")); // Close the connection with poison pill
            } else {
                response.push_back(string("What?\n")); // Respond with "Pong!" 
            }
            //    response.push_back(string("CLOSE")); // Close the connection with poison pill
            break;
        case 2:
            if( cmd[0] == "GET" ) {
                cout << "GET " << cmd[1] << endl;
            }
            break;
        case 3:
            if( cmd[0] == "SET" ) {
                cout << "SET " << cmd[1] + " to " + cmd[2] << endl;
            }
            break;
    }
    
    return response;
}

// void handleConnection(int newsockfd, sockaddr_in* cli_addr) {
void *handleConnection(void *xfer) {
    struct toThread *ptr = (struct toThread *)xfer ;
    int newsockfd; 
    sockaddr_in *cli_addr;
    
    newsockfd = ptr->newsockfd;
    cli_addr = ptr->cli_addr;
    
    char buffer[256]; // Initialize buffer to zeros
    bzero(buffer, 256);
    
    while (true) {
        int n = read(newsockfd, buffer, 255);
        if (n == 0) {
            cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
            << " connection closed by client" << endl;
            return (void *)NULL;
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
                        if (n < 0) {
                            cerr << "ERROR writing to socket" << endl;
                        }
                    }
                    else {
                        close(newsockfd); // Close the connection if response line == "CLOSE"
                        cout << inet_ntoa(cli_addr->sin_addr) << ":" << ntohs(cli_addr->sin_port)
                        << " connection terminated" << endl;
                        return (void *)NULL;
                    }
                }
            }
        }
    }
    return (void *)NULL;
}

int main(int argc, const char *argv[]) {
    int sockfd; // Socket file descriptor
    int portno; // Port number
    
    sockaddr_in serv_addr; // Server address
    
    if (argc < 2) {
        cerr << "ERROR no port provided" << endl;
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create new socket, save file descriptor
    if (sockfd < 0) {
        cerr << "ERROR opening socket" << endl;
    }
    
    int reusePort = 1; // Disables default "wait time" after port is no longer in use before it is unbound
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
    
    bzero((char *) &serv_addr, sizeof(serv_addr)); // Initialize serv_addr to zeros
    portno = atoi(argv[1]); // Reads port number from char* array
    
    serv_addr.sin_family = AF_INET; // Sets the address family
    serv_addr.sin_port = htons(portno); // Converts number from host byte order to network byte order
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Sets the IP address of the machine on which this server is running
    
    if (bind(sockfd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // Bind the socket to the address
        cerr << "ERROR on binding" << endl;
    
    unsigned int backlogSize = 5; // Number of connections that can be waiting while another finishes
    listen(sockfd, backlogSize);
    cout << "C++ server opened on port " << portno << endl;;
    
    pthread_t thread_id;

    while (true) {
        int newsockfd; // New socket file descriptor
        unsigned int clilen; // Client address size
        sockaddr_in cli_addr; // Client address
        
        clilen = sizeof(sockaddr_in);
        newsockfd = accept(sockfd, (sockaddr *) &cli_addr, &clilen); // Block until a client connects
        if (newsockfd < 0)
            cerr << "ERROR on accept" << endl;
        
        cout << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port)
        << " connected" << endl;
        
        struct toThread *ptr = (struct toThread *) calloc(1, sizeof(struct toThread));
        
        ptr->newsockfd = newsockfd;
        ptr->cli_addr = &cli_addr;
        
//        handleConnection(ptr); // Handle the connection
        if( pthread_create( &thread_id , NULL ,  handleConnection , (void *)ptr) < 0) {
            perror("could not create thread");
            return 1;
        }
    }
    return 0;
}
