#include "timerBase.h"
#include <unistd.h>


int main() {
    int delay=1000;
    msTimer *t1 = new msTimer(delay);

    bool rf=true;

    bool res=false;

    int i=1;

    while ( res == false) {
        res = t1->run( rf );

        usleep(2000) ;
        printf("i=%d\n", i++);

        if ( i == 100 ) {
            rf = false;
        } else {
            rf = true;
        }
        printf("res = %d\n", res);
        printf("rf  = %d\n", rf);
    }

}

