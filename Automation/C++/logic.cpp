
#include "plc.h"
#include "myClientClass.h"
#include "semaphore.h"

#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <signal.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>
#include <vector>

sqlite3 *db;

myClient *me;

uint32_t signalCount=0;
int sid;
bool runFlag ;

void alarmHandler(int sig) {
    signal(SIGALRM, SIG_IGN);
    printf("Alarm fired\n");
    signalCount++;
    //    signal(SIGALRM, alarmHandler);
}

void hupHandler(int sig) {
    signal(SIGHUP, SIG_IGN);
    printf("HUP fired\n");
    signalCount++;
    runFlag=false;
    signal(SIGHUP, hupHandler);
}

void decrementSignalCount() {
    if( signalCount > 0) {
        signalCount--;
    }
}

void usage() {
    printf("Help\n");
    
    printf("\t-c\tCheck program\n");
    printf("\t-h\tHelp\n");
    printf("\t-v\tVerbose\n");
    printf("\t-p <file>\tProgram file\n");
}

int main(int argc, char *argv[]) {
    int rc;
    string fileName;
    bool verbose=false;
    
    int key=42;
    
    /*
     *    if( argc != 2 ) {
     *        usage();
     *        exit(1);
}

fileName = argv[1];

*/
    string myName = basename(argv[0]);
    
    ifstream progFile;
    bool check=false;
    
    int opt;
    while ((opt = getopt(argc, argv, "chp:v")) != -1) {
        switch(opt) {
            case 'c':
                check = true;
                break;
            case 'h':
                usage( );
                exit(0);
                break;
            case 'p':
                fileName = optarg;
                break;
            case 'v':
                verbose=true;
                break;
        }
    }
    
    progFile.open( fileName.c_str() );
    
    if( progFile.fail()) {
        fprintf(stderr,"Failed to open %s\n", fileName.c_str());
        exit(1);
    }
    
    string line;
    vector<void *> prog;
    int outputPid;
    
    bool gotOutputsPid=false;
    
    if( check == false ) {
        
        do {
            FILE *fd = fopen("/var/tmp/updateOutputs", "r");
            
            if( fd == NULL ) { 
                printf("Can't open updateOutputs pid file, sleeping .... zzz\n");
                sleep(5);
            } else {
                char pidBuffer[16];
                char *rc = fgets( pidBuffer, 16, fd);
                fclose( fd );
                outputPid = atoi( pidBuffer );
                
                int ok = kill(outputPid, 0); 
                
                gotOutputsPid = ( ok == 0) ? true : false ;
                
                if( gotOutputsPid == false ) { 
                    printf("Can't contact updateOutputs, sleeping .... zzz\n");
                    sleep(5);
                }
            }
        } while(gotOutputsPid == false) ;
    }
    
    
    instruction *thing;
    bool isaTimer = false;
    char *comment;
    while(getline(progFile, line)) {
        isaTimer=false;
        
        cout << line << endl;
        char *l = (char *)line.c_str();
        
        comment = strsep(&l,";");
        
        char *inst = strtok( (char *)line.c_str(), " \t");
        if( inst == NULL) {
            continue;
        }
        
        char *var = strtok( NULL, " \t");
        //
        // Check if it's one of the LD instructions
        //
        if(!strcmp( inst, "LD")) {
            thing = new ld(var );
        } else if(!strcmp( inst, "LDN")) {
            thing = new ldn(var );
        } else if(!strcmp( inst, "LDR")) {
            thing = new ldr(var );
        } else if(!strcmp( inst, "LDF")) {
            thing = new ldf(var );
        } else if(!strncmp( inst, "OUT", 3)) {
            if(inst[3] == '\0' ) {
                thing = new out(var );
            } else if( inst[3] == 'N') {
                thing = new outn(var );
            }
        } else if(!strncmp( inst, "OR", 2)) {
            if(inst[2] == '\0' ) {
                thing = new Or(var );
            } 
        } else if(!strncmp( inst, "AND", 3)) {
            if(inst[3] == '\0' ) {
                thing = new And(var );
            } else if(inst[3] == 'N' ) {
                thing = new Andn(var );
            }
        } else if(!strncmp( inst, "END", 3)) {
            break;
        } else if(!strcmp( inst, "TIM-LD")) {
            thing = new timLd(var );
        } else if(!strcmp( inst, "TIM-LDN")) {
            thing = new timLdn(var );
        } else if(!strcmp( inst, "TIM-AND")) {
            thing = new timAnd(var );
        } else if(!strcmp( inst, "TIM-ANDN")) {
            thing = new timAndn(var );
        } else {
            thing = new Noop(NULL);
        }
        prog.push_back( thing );
    }
    
    progFile.close();
    
    if( check) {
        for (auto i : prog ) {
            instruction *y = (instruction *)i;
            y->dump();
        }
        exit(0);
    }
    
    string hostName = "localhost";
    string serviceName ="myclient" ;
    
    //    myClient *n = myClient::Instance();
    me = myClient::Instance();
    
    bool failFlag = me->setupNetwork((char *)hostName.c_str(), (char *)serviceName.c_str());
    if(verbose) {
        printf("Network setup %s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    } 
    
    failFlag = me->loadFile( "mysqlCmds.txt");
    if(verbose) {
        cout << "loadFile ";
        printf("%s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }
    
    failFlag = me->clientConnect() ;
    
    if(verbose) {
        cout << "clientConnect ";
        printf("%s\n", ((failFlag) ? (char *)"failed" : (char *)"success"));
    }
    
    if ( me->clientConnected() == false ) {
        fprintf(stderr, "FATAL ERROR: Connected to interface but not to database\n");
        exit(2);
    }
    
    time_t now=0;
    struct tm *hms;
    int delay =0;
    int hours=0;
    int minutes=0;
    int seconds=0;
    
    while(true) {
        runFlag = true;
        pid_t iam=getpid();
        
        string pidFile = "/var/tmp/" + myName;
        
        FILE *fd = fopen( pidFile.c_str(), "w");
        
        if( fd == NULL ) {
            fprintf(stderr, "FATAL ERROR: Failed to open pidFile\n");
            exit(3);
        }
        
        fprintf(fd, "%d\n" , iam);
        fclose(fd);
        
        sid = semtran( key );
        signal(SIGALRM, alarmHandler);
        signal(SIGHUP, hupHandler);
        
        while( runFlag) {
            now = time(NULL);
            hms = localtime( &now );
            hours = hms->tm_hour ;
            minutes = hms->tm_min ;
            seconds = hms->tm_sec ;
            
            delay = 61 - seconds ;
            
            if( verbose ) {
                printf("Time is %02d:%02d\n", hours, minutes);
                printf("Wait for %d seconds\n", delay);
            }
            
            if ( signalCount == 0 ) {
                signal(SIGALRM, alarmHandler);
                alarm(delay);
                sleep(delay-1);
            } else {
                alarm(0);
            }
            
            getSem(sid);
            
            if( verbose ) {
                printf("Signal count %d\n", signalCount);
            }
            
            for (auto i : prog ) {
                instruction *y = (instruction *)i;
                y->dump();
                y->act();
            }
            
            decrementSignalCount();
            relSem(sid);
            
            printf("signal count %d\n", signalCount);
            
            kill(outputPid, SIGUSR1);
            
        }
    }
}
