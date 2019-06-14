#include <nlohmann/json.hpp>
#include <unistd.h>

class plcSocket : public plcBase {
    private:
        string cfgFile = "/etc/mqtt/bridge.json";
        string socketServer = "localhost";
        int portNo = 9191;

        bool instanceFailed = true;
        json config;
        int serverSock;

        const void plcSetup(string cfg);
    public:
        plcSocket();
        plcSocket(string cfgFile);

        void setServerSock(int);

//        void setVerbose(bool flag);

        string getValue(string shortName);  // get from socket server defined in cfg file.
        bool getBoolValue(string name);

        void setValue(string shortName, string value);
        void setBoolValue(string shortName, bool flag);

        void dump();
        // 
        // PLC Stuff
        //
        void Ld(string symbol);
        void Out(string symbol);
} ; 


