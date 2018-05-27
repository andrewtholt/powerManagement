#pragma once 
union semun {
    int             val;    /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;  /* array for GETALL, SETALL */
    /* Linux specific part: */
    struct seminfo *__buf;  /* buffer for IPC_INFO */
};

int semtran(int key) ;
int semcall(int sid, int op) ;
int getSem(int sid) ;
int relSem(int sid) ;
int getSemValue(int sid) ;
int setSemValue(int sid, int val) ;

