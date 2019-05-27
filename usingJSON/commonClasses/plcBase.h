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
		int fd;

		stack<bool> logicStack;

		map<string, string> ioPoint;   // name, value

		bool cacheHit = true;

    protected:
		bool verbose=false;
	public:
//		plcBase(int fd);
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
		plcBase();

    	void cacheDump();

    	void setVerbose(bool );

		bool Pop();     // Remove and return top stack entry
        void Push(bool v); // Push to stack.
        void display();     // Display tos
		void Ld(string symbol);
		void Or();
		void And();
		void Invert();

		void Out(string symbol);
		void End(int delay);

};
