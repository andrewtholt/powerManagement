#include <nlohmann/json.hpp>
#include <unistd.h>

class plcSocket : protected plcBase {
    private:
        string cfgFile = "/etc/mqtt/bridge.json";
        string socketServer = "localhost";
        int portNo = 9191;

        bool instanceFailed = true;

        json config;

    public:
        plcSocket();
        plcSocket(string cfgFile);

        void setVerbose(bool flag);
        string getValue(string shortName);  // get from socket server defined in cfg file.
        void setValue(string shortName, string value);
} ; 


