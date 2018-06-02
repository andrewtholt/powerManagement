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
    printf("\t[%d]\t%s\t%s\n",state, iName, name);
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
}
// 
// LDR Load a value, set the state true on rising edge
//
ldr::ldr(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LDR");
}

void ldr::act() {
    getIoPoint( name );
    current=flag;
    state = (( old == false ) && ( current == true )) ? true : false ;
    old = current;
}

void ldr::dump() {
    instruction::dump((char *)"LDR");
}
// 
// LDF Load a value, set the state true on falling edge
//
ldf::ldf(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LDR");
}

void ldf::act() {
    getIoPoint( name );
    current=flag;

    state = (( old == true ) && ( current == false )) ? true : false ;

    old = current;
}

void ldf::dump() {
    instruction::dump((char *)"LDF");
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
// OUTN take a bool, invert it and set the output.
//
outn::outn(char *n) {
    strncpy(name, n, sizeof(name));
}

void outn::act() {
    setIoPoint( name, !state );
    state=false;
}

void outn::dump() {
    instruction::dump((char *)"OUTN");
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
// ORN
//
Orn::Orn(char *n) {
    strncpy(name, n, sizeof(name));
}

void Orn::act() {
    getIoPoint( name);
    state = state || !flag;
}

void Orn::dump() {
    instruction::dump((char *)"ORN");
}
// 
// ORR 
//
Orr::Orr(char *n) {
    strncpy(name, n, sizeof(name));
}

void Orr::act() {
    getIoPoint( name);
    bool old;
    
    bool current=flag;
    
    bool tmp = (( old == false ) && ( current == true )) ? true : false ;
    old = current;
    
    state = state || tmp;
}

void Orr::dump() {
    instruction::dump((char *)"ORR");
}
// 
// ORF 
//
Orf::Orf(char *n) {
    strncpy(name, n, sizeof(name));
}

void Orf::act() {
    getIoPoint( name);
    bool old;
    
    bool current=flag;
    
    bool tmp = (( old == true ) && ( current == false )) ? true : false ;
    old = current;
    
    state = state || tmp;
}

void Orf::dump() {
    instruction::dump((char *)"ORF");
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
// ANDR 
//
Andr::Andr(char *n) {
    strncpy(name, n, sizeof(name));
}

void Andr::act() {
    getIoPoint( name);
    bool old;
    
    bool current=flag;
    
    bool tmp = (( old == false ) && ( current == true )) ? true : false ;
    old = current;
    
    state = state && tmp;
}

void Andr::dump() {
    instruction::dump((char *)"ANDR");
}
// ANDF 
// 
void Andf::act() {
    getIoPoint( name);
    bool old;
    
    bool current=flag;
    
    bool tmp = (( old == true ) && ( current == false )) ? true : false ;
    old = current;
    
    state = state && tmp;
}

void Andf::dump() {
    instruction::dump((char *)"ANDR");
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
// 
// TIM-LD When the time matches set time return true, else false.
//
timLd::timLd(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LD");
}

void timLd::act() {
    time_t now=time(NULL);
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    char target[6];  // hh:mm

    strcpy( target, name );

    char *tHours = strtok(target,":");
    char *tMins = strtok(NULL,":");

    state = ((hours == atoi(tHours)) && (minutes == atoi(tMins))) ? true : false ;    
}

void timLd::dump() {
    instruction::dump((char *)"TIM-LD");
}
// 
// TIM-LDN When the time matches set time return false, else true.
//
timLdn::timLdn(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LD");
}

void timLdn::act() {
    time_t now=time(NULL);
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    char target[6];  // hh:mm

    strcpy( target, name );

    char *tHours = strtok(target,":");
    char *tMins = strtok(NULL,":");

    state = ((hours == atoi(tHours)) && (minutes == atoi(tMins))) ? false : true ;    
}

void timLdn::dump() {
    instruction::dump((char *)"TIM-LD");
}
// 
// TIM-ANDN When the time matches set time 
//
timAndn::timAndn(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LD");
}

void timAndn::act() {
    time_t now=time(NULL);
    bool flag = false;
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    char target[6];  // hh:mm

    strcpy( target, name );

    char *tHours = strtok(target,":");
    char *tMins = strtok(NULL,":");

    flag = ((hours == atoi(tHours)) && (minutes == atoi(tMins))) ? false : true ;    

    state = state & !flag;
}

void timAndn::dump() {
    instruction::dump((char *)"TIM-ANDN");
}
// 
// TIM-AND When the time matches set time 
//
timAnd::timAnd(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LD");
}

void timAnd::act() {
    time_t now=time(NULL);
    bool flag = false;
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    char target[6];  // hh:mm

    strcpy( target, name );

    char *tHours = strtok(target,":");
    char *tMins = strtok(NULL,":");

    flag = ((hours == atoi(tHours)) && (minutes == atoi(tMins))) ? false : true ;    

    state = state & flag;
}

void timAnd::dump() {
    instruction::dump((char *)"TIM-AND");
}
// 
// TIM-ORN When the time matches set time 
//
timOrn::timOrn(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"LD");
}

void timOrn::act() {
    time_t now=time(NULL);
    bool flag = false;
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    char target[6];  // hh:mm

    strcpy( target, name );

    char *tHours = strtok(target,":");
    char *tMins = strtok(NULL,":");

    flag = ((hours == atoi(tHours)) && (minutes == atoi(tMins))) ? false : true ;    

    state = state || !flag;
}

void timOrn::dump() {
    instruction::dump((char *)"TIM-ORN");
}
// 
// TIM-OR When the time matches set time 
//
timOr::timOr(char *n) {
    extern sqlite3 *db;
    strncpy(name, n, sizeof(name));
    strcpy(id,(char *)"TIM-OR");
}

void timOr::act() {
    time_t now=time(NULL);
    bool flag = false;
    struct tm *hms = localtime( &now );
    int hours = hms->tm_hour ;
    int minutes = hms->tm_min ;
    char target[6];  // hh:mm

    strcpy( target, name );

    char *tHours = strtok(target,":");
    char *tMins = strtok(NULL,":");

    flag = ((hours == atoi(tHours)) && (minutes == atoi(tMins))) ? false : true ;    

    state = state || flag;
}

void timOr::dump() {
    instruction::dump((char *)"TIM-OR");
}





