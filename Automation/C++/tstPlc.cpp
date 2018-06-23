
#include <iostream>
#include "plc.h"

int main(int argc, char *argv[]) {

    plc test = plc("testing","raspberrypi0");

    test.plcDump();

    dump();

    test.loadProg((char *)argv[1]);
}
