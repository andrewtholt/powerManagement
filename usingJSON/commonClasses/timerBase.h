#include <stdint.h>

using namespace std;

class msTimer {
    private:
        uint32_t startTime=0;
        uint32_t elapsedTime=0;
        uint32_t duration;

        bool output=false;
        bool running=false;

    public:
        msTimer(int ms);
        void start();
        int32_t read();
        bool run(bool flag);
};
