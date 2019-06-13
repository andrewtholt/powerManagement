#include "timerBase.h"
#include <unistd.h>


int main() {
    int delay=1000;
    msTimer *t1 = new msTimer(delay);

    t1->setOneShot(true);

    bool rf=true;

    bool res=false;

    int i=1;

//    while ( res == false) {
    while ( true) {
        res = t1->run( rf );
        printf("res = %d\n", res);

        if( res ) {
            printf("Time\n");
        }

        usleep(4000) ;
        printf("i=%d\n", i++);

        if ( i == 100 ) {
            rf = false;
        } else {
            rf = true;
        }
        printf("rf  = %d\n", rf);
    }

}

