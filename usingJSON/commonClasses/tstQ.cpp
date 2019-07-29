#include "tQueue.h"

using namespace std;

int main() {
    tQueue Q;
    string tst;

    cout << "Push ONE" << endl;
    Q.push("ONE");
    cout << Q.depth() << endl; 

    cout << "Get ONE" << endl;
    tst = Q.get();

    cout << tst << endl;
    cout << Q.depth() << endl; 
    
    cout << "Push TWO" << endl;
    Q.push("TWO");
    cout << Q.depth() << endl; 

    cout << "Pop ONE" << endl;
    tst = Q.pop();
    cout << tst << endl; 
    cout << Q.depth() << endl; 
}


