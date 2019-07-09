#include <vector>
#include <string>
#include <nlohmann/json.hpp>

std::string getFromJson(nlohmann::json j, std::vector<std::string> p, std::string defaultValue) ;

std::string addOffset(int hh, int mm, int off) ;

uid_t getUserIdByName(const char *name);

