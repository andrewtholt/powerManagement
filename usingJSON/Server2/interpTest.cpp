#include <iostream>
#include <string>

#include "interp.h"

using namespace std;

int main () {
    interp first;
    string result;

    string p1,p2;

//    result=first.runCmd("ping","","");
    result=first.runCmd((vector<string>){"PING"});

    cout << result << endl;
}

