#include "haRest.h"

int main() {

    haRest *rest = new haRest("192.168.10.124",8123);

//    rest->setHaPort(9292);

    rest->dump();

    rest->get("switch.test_start");
}

