#include <stdint.h>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <map>
#include <vector>
#include <mysql/mysql.h>

// typedef  std::string (*methodPtr)(std::vector<std::string> c);
typedef  std::string (*methodPtr)(std::vector<std::string> c);
enum opTypeDef { JSON, SIMPLE };

class interp {
    private:
        MYSQL *conn;
        std::mutex mtx;
        opTypeDef opType = JSON;
        std::map<std::string, std::string> localVariable;
        std::string getRemoteVariable(std::string);
        std::string setRemoteVariable(std::string, std::string);

    public:
        interp();
        interp(MYSQL *);
        std::string Ping(std::vector<std::string>);
        std::string Close(std::vector<std::string>);
        std::string undefinedCmd(std::vector<std::string>);

        std::string Get(std::vector<std::string>);
        std::string Set(std::vector<std::string>);

        std::string runCmd(std::vector<std::string>);
};

