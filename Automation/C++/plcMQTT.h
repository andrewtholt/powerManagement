
#pragma once

/*
#include <sqlite3.h>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <hspread.h>
*/

#include "plcMQTT.h"
#include "plcBase.h"

using namespace std;


class plcMQTT : public plcBase 
{
    public:
        plcMQTT() ;

        void initPlc() ;
};
