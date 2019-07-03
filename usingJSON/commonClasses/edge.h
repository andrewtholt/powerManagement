#include <stdint.h>
#include <iostream>


using namespace std;

enum TRANSITION { RISING=0, FALLING };

class edge {
    private:
        bool output=false;
        bool flag=false;

        TRANSITION whichEdge;

        bool positive(bool in);
        bool negative(bool in);
    public:
        edge(TRANSITION t);
        bool run(bool in);
};
