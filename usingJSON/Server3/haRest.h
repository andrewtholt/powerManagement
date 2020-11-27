#include <iostream>

class haRest {

    private:
        std::string haHostIp = "";
        int port = 0;

        std::string haTokenFilename = "/etc/mqtt/haToken.txt";
        std::string haToken = "";

        void commonInit();

    public:
        haRest();
        haRest(std::string haHost);
        haRest(std::string haHost, int port);

        void setHaHost(std::string ip);
        void setHaPort(int p);

        std::string get(std::string entityId);
        std::string set(std::string entityId, std::string state);

        void dump();
};

