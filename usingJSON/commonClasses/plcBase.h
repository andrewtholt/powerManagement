#pragma once

#include <iostream>
#include <string>
#include <stack>
#include <map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef __cplusplus
extern "C" {
// struct plcBase *newPlcBase(spreadBase *spr);

#endif

#ifdef __cplusplus
}
#endif

using namespace std;

class plcBase {

    private:
		bool verbose=false;
		stack<bool> logicStack;

		int fd;
		map<string, string> ioPoint;   // name, value

		bool cacheHit = true;

        string getValue( string shortName);
        string getOldValue( string );
        void setValue(string shortName, string v );
        bool setOldValue(string shortName, string v );
        // 
        // Syntactic sugar.
        //
        bool getBoolValue( string );
        bool setBoolValue(string shortName, bool v );
        bool getBoolOldValue( string );
        bool setBoolOldValue(string shortName, bool v );


	public:
//		plcBase(int fd);
		plcBase();
		~plcBase();
    	void cacheDump();

    	void setVerbose(bool );

		bool Pop();     // Remove and return top stack entry
        void Push(bool v); // Push to stack.
		void Ld(string symbol);
		void Or();
		void And();
		void Invert();

		void Out(string symbol);
		void End(int delay);

};
