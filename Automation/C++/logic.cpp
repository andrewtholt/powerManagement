
#include <iostream>
#include "plc.h"
#include <unistd.h>
#include <libgen.h>


void usage() {
    printf("\t-h|-?\tHelp\n");
    printf("\t-p<program>\tName of plc program file.\n");
    printf("\t-s<spread host>\tName or IP of spread host.\n");
}

int main(int argc, char *argv[]) {
    
    int opt;
    string spreadHost = "localhost";
    string plcProg;
    bool verbose=false;
    string myName = basename(argv[0]);
    
    while ((opt = getopt(argc, argv, "?hp:s:v")) != -1) {
            switch(opt) {
                case 'h':
                case '?':
                    usage();
                    exit(0);
                    break;
                case 'p':
                    plcProg = optarg;
                    break;
                case 's':
                    spreadHost = optarg;
                    break;
                case 'v':
                    verbose=true;
                    break;
            }
    }


    plc test = plc(myName,spreadHost);

    test.plcDump();

//    dump();

    test.setVerbose(verbose);
    test.loadProg( plcProg);
    
//    test.dumpProgram();
    
    test.plcRun();
}
