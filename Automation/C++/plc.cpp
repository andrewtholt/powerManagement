#include "myClientClass.h"
#include <string.h>
#include "plc.h"
#include <stdio.h>
#include <iostream>


static bool state;
static bool flag;

static int getLogicCallback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;

    /*
    for(i=0; i<argc; i++){
        printf("[%d] %s = %s\n", i, azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    */

    if(!strcmp( argv[0], argv[1])) {
        flag = true;
    } else if(!strcmp( argv[0], argv[2])) {
        flag = false;
    }

    return 0;
}

void getIoPoint( char *n) { 
    extern myClient *me;
    int rc;

    int len=-1;
    string out;

    static char cmd[255];
    char *sqlTemplate = (char *)"select state, on_state, off_state from io_point where name = '%s';\n";

    sprintf(cmd,sqlTemplate, n);

    len = me->sendCmd( cmd, out );

    string state;
    string on_state;
    string off_state;

    strcpy(cmd, (char *)"^get-col state\n");
    len = me->sendCmd( cmd, state );

    strcpy(cmd, (char *)"^get-col on_state\n");
    len = me->sendCmd( cmd, on_state );

    strcpy(cmd, (char *)"^get-col off_state\n");
    len = me->sendCmd( cmd, off_state );

    flag = ( state == on_state ) ? true : false ;
}

void setIoPoint( char *n, bool s) { 
    extern myClient *me;

    static char cmdBuffer[255];

    int len=0;

    char *sqlTemplate = (char *)"update io_point set state = %s where name = '%s';\n";

    if ( s == true) {
        sprintf(cmdBuffer, sqlTemplate, "on_state", n);
    } else {
        sprintf(cmdBuffer, sqlTemplate, "off_state", n);
    }
    printf("%s\n", cmdBuffer);
    len = me->sendCmd( cmdBuffer );
}


void instruction::dump(char *iName) {
    printf("\t[%d]\n\t%s\t%s",state, iName, name);
}

// 
// LD Load a value, set the state
//
ld::ld(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LD");
}

void ld::act() {
    getIoPoint( name );
    state = flag;

}

void ld::dump() {
    instruction::dump((char *)"LD");
//    instruction::dump((char *)name);
}

// 
// LDN Load a value, invert and set ths state
//
ldn::ldn(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
}

void ldn::act() {
    getIoPoint( name );

    state = (flag == true) ? false : true ;

}

void ldn::dump() {
    instruction::dump((char *)"LDN");
}
// 
// OUT take a bool and set the output.
//
out::out(char *n) {
    strncpy(name, n, sizeof(name));
}

void out::act() {
    setIoPoint( name, state );
    state=false;
}

void out::dump() {
    instruction::dump((char *)"OUT");
    cout << endl;
}
// 
// OR 
//
Or::Or(char *n) {
    strncpy(name, n, sizeof(name));
}

void Or::act() {
    getIoPoint( name);
    state = state || flag;
}

void Or::dump() {
    instruction::dump((char *)"OR");
}
// 
// AND 
//
And::And(char *n) {
    strncpy(name, n, sizeof(name));
}

void And::act() {
    getIoPoint( name);
    state = state && flag;
}

void And::dump() {
    instruction::dump((char *)"ANDN");
}
// 
// ANDN 
//
Andn::Andn(char *n) {
    strncpy(name, n, sizeof(name));
}

void Andn::act() {
    getIoPoint( name);
    state = state && !flag;
}

void Andn::dump() {
    instruction::dump((char *)"ANDN");
}
// 
// NOOP 
//
Noop::Noop(char *n) {
//    strncpy(name, n, sizeof(name));
}

void Noop::act() {
}

void Noop::dump() {
    instruction::dump((char *)"NOOP");
}





