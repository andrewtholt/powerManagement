#include <stdio.h>
#include "timerBase.h"
#include <unistd.h>

int main() {
    msTimer *n = new msTimer();

    n->start();
    usleep(2000);

    printf("%d\n", n->read());
}
