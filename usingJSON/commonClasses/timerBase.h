#include <stdint.h>
#include <iostream>


using namespace std;

class msTimer {
    private:
        uint32_t startTime=0;
        uint32_t elapsedTime=0;
        uint32_t duration;

        bool output=false;
        bool running=false;
        bool oneShot = false;
        bool triggered = false;
    public:
//        msTimer(int ms) { cout << "Hello " << endl ;};
        msTimer(int ms) ;
        void timerStart(void);
        int32_t read();
        bool run(bool flag);

        void setOneShot(bool flag);
};
