#include <stdint.h>
#include <iostream>


using namespace std;

class toggle {
    private:
        bool output=false;
        bool flag=false;
    public:
        bool run(bool in);
};
