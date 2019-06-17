#include "toggle.h"

#include <iostream>

using namespace std;

int main() {
    bool in=false;
    bool out=false;
    toggle *t = new toggle();


    out=t->run( in ) ;
    cout << "In  : " << in  << endl;
    cout << "Out : " << out << endl;
    cout << "======" <<endl;

    in = true;
    out=t->run( in ) ;
    cout << "In  : " << in  << endl;
    cout << "Out : " << out << endl;
    cout << "======" <<endl;

    in = false;
    out=t->run( in ) ;
    cout << "In  : " << in  << endl;
    cout << "Out : " << out << endl;
    cout << "======" <<endl;

    in = true;
    out=t->run( in ) ;
    cout << "In  : " << in  << endl;
    cout << "Out : " << out << endl;
    cout << "======" <<endl;

    in = false;
    out=t->run( in ) ;
    cout << "In  : " << in  << endl;
    cout << "Out : " << out << endl;
    cout << "======" <<endl;
}

