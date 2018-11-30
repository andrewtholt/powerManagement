
#include "plcBase.h"
#include "plcMQTT.h"

plcMQTT::plcMQTT() {
    printf("plcMQTT\n");
    hostName = "127.0.0.1";
    port = 1883;
}

bool plcMQTT::connect() {
    bool failFlag = true;

    return failFlag;
}
void plcMQTT::initPlc() {
}

