#include <stdint.h>
#include <iostream>
#include <mutex>

class incCounter {
    private:
        long count=0;
        std::mutex mtx;

    public:
        void increment();
        void decrement();
        long get();
        void reset();
};
