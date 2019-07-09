#include <vector>
#include <string>
#include <nlohmann/json.hpp>

std::string getFromJson(nlohmann::json j, std::vector<std::string> p, std::string defaultValue) ;

std::string addOffset(int hh, int mm, int off) ;

uid_t getUserIdByName(const char *name);

std::vector<std::string> split(const char *str, char c=' ') ;

inline std::string &ltrim(std::string& s, const char* t = " \t\n\r\f\v") ;
inline std::string &rtrim(std::string& s, const char* t = " \t\n\r\f\v") ;
std::string &trim(std::string& s, const char* t = " \t\n\r\f\v") ;

