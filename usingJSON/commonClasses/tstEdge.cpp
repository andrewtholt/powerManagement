#include "edge.h"

#include <iostream>

using namespace std;

void outputLogic(bool i) {
    cout << "State:   ";
    if( i == true  ) {
        cout << "TRUE";
    } else {
        cout << "FALSE";
    }

    cout << endl;
}

int main() {
    bool in=false;
    bool out=false;
    edge *t = new edge(RISING);
    edge *t1 = new edge(FALLING);


    cout << "POSITIVE EDGE" << endl;
    out=t->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = true;
    out=t->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = false;
    out=t->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = true;
    out=t->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = true;
    out=t->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = false;
    out=t->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    cout << "NEGATIVE EDGE" << endl;

    in = false;
    out=t1->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = true;
    out=t1->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = true;
    out=t1->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = false;
    out=t1->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = false;
    out=t1->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

    in = true;
    out=t1->run( in ) ;
    cout << "In   : " << in  << endl;
    outputLogic(out) ;
    cout << "======" <<endl;

}

