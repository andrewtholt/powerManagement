
#include "plc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <vector>

sqlite3 *db;

void usage() {
    printf("Help\n");
}

int main(int argc, char *argv[]) {
    int rc;
    char *fileName;

    if( argc != 2 ) {
        usage();
        exit(1);
    }

    fileName = argv[1];

    ifstream progFile;

    progFile.open( fileName );

    if( progFile.fail()) {
        fprintf(stderr,"Failed to open %s\n", fileName);
        exit(1);
    }

    string line;
    vector<void *> prog;

    instruction *thing;
    while(getline(progFile, line)) {
        cout << line << endl;
        char *inst = strtok( (char *)line.c_str(), " \t");
        char *var = strtok( NULL, " \t");
        //
        // Check if it's one of the LD instructions
        //
        if ( inst[0] == 'L' && inst[1] == 'D') {
            if( inst[2] == '\0') {
                thing = new ld(var );
            } if( inst[2] == 'N') {
                thing = new ldn(var );
            } 
        } else if(!strncmp( inst, "OUT", 3)) {
            if(inst[3] == '\0' ) {
                thing = new out(var );
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
        }
        prog.push_back( thing );
    }

    progFile.close();

    rc = sqlite3_open("/var/tmp/automation.db", &db);

    if( rc ) {
        fprintf(stderr,"Failed to open database.\n");
        exit(1);
    }

    for (auto i : prog ) {
        instruction *y = (instruction *)i;
        y->dump();
        y->act();

    }


}
