#include "haRest.h"

int main() {

    std::string result="";

    haRest *rest = new haRest("192.168.10.124",8123);

//    rest->setHaPort(9292);

    rest->dump();

    rest->setReturnStateOnly(true);
    rest->setVerbose(true);

    result = rest->get("switch.test_start");

    std::cout << "Get Result : " << result << std::endl;

    result = rest->set("switch.christmas_lights","on");

    std::cout << "Get Result : " << result << std::endl;
}

