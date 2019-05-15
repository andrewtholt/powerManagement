#include <nlohmann/json.hpp>

class plcSocket : protected plcBase {
    private:
        string cfgFile = "/etc/mqtt/bridge.json";
        string socketServer = "localhost";
        int portNo = 9191;

    public:
        plcSocket();
        plcSocket(string cfgFile);

        void setVerbose(bool flag);
        string getValue(string shortName);  // get from socket server defined in cfg file.
} ; 


