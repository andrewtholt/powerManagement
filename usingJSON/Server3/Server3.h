#include <stdbool.h>

struct globalDefinitions {
    bool debug;
    bool verbose;
    bool redis;
    bool raw;

    std::string haIP;
    int haPort;
    int myPort;
};
