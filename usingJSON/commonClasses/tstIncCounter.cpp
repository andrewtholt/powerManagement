
#include "incCounter.h"

using namespace std;

int main() {
    incCounter tst;

    cout << "init:" << tst.get() << endl;

    tst.increment();
    cout << "inc :" << tst.get() << endl;

    tst.decrement();
    cout << "dec :" << tst.get() << endl;

    tst.decrement();
    cout << "dec :" << tst.get() << endl;
}

