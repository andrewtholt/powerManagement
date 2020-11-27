#include "haRest.h"

int main() {

    std::string result="";

    haRest *rest = new haRest("192.168.10.124",8123);

//    rest->setHaPort(9292);

    rest->dump();

    result = rest->get("switch.test_start");

    std::cout << "Result : " << result << std::endl;
}

