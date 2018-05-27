#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "semaphore.h"

int semtran(int key) {
    int sid;

    if( key <0) {
        sid=-1;
    } else {
        sid = semget(key, 1, 0666 | IPC_CREAT);
    }
    return sid;
}

int semcall(int sid, int op) {
    struct sembuf   sb;  

    sb.sem_num = 0; 
    sb.sem_op = op;
    /*   
     * sb.sem_flg = IPC_NOWAIT;
     *             */
    sb.sem_flg = 0; 

    if (semop(sid, &sb, 1) == -1) {
        perror("ficl: semcall ");
        return (-1);
    } else {
        return (0); 
    }    
}

int getSem(int sid) {
    return( semcall(sid, -1) );
}

int relSem(int sid) {
    return( semcall(sid, 1) );
}

int getSemValue(int sid) {
    int res;

    res = semctl(sid, 0, GETVAL);

    return res;
}

int setSemValue(int sid, int val) {

    union semun arg;

    arg.val = val;

    int res = semctl(sid, 0, SETVAL, arg);

    res = (res == 0) ? -1 : 0;

    return res;
}
