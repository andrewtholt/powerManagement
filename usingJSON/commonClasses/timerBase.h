#include <stdint.h>

using namespace std;

class msTimer {
    private:
        uint32_t startTime=0;
        uint32_t elapsedTime=0;

    public:
        void start();
        uint32_t read();

};
