#include <iostream>
#include <string>
#include <map>

#include "interp.h"

using namespace std;

time_t startTime = 0;
map<string, string> globalVariable;

int main () {
    interp first;
    string result;

    string p1,p2;

//    result=first.runCmd("ping","","");
    result=first.runCmd((vector<string>){"PING"});

    cout << result << endl;
}

