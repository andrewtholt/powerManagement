
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

#include <sys/types.h>
#include <pwd.h>

using json = nlohmann::json;

using namespace std;

string getFromJson(json j, vector<string> p, string defaultValue) {

    string v;
    string t;
    int len = p.size();

    try {
        switch(len) {
            case 0:
                break;
            case 1:
                v=j[ p[0]  ];
                break;
            case 2:
                {
                    string a = p[0];
                    string b = p[1];

                    v=j[a][b];
                }
                break;
            case 3:
                v=j[p[0]] [p[1]] [p[2]];
                break;
            default: 
                cerr << "getFromJson:FATAL Error, too many parameters, Max is 3" << endl;
                exit(4);
                break;
        }
    } catch (json::type_error &e) {
        v = defaultValue;
    }

    return v;
}

string addOffset(int hh, int mm, int off) {

    int ahh, amm;

    int minutes = (hh * 60) + mm;

    minutes += off;

    ahh = minutes / 60;
    amm = minutes % 60;

    string res = to_string(ahh) + ":" + to_string(amm);

    return res;
}

uid_t getUserIdByName(const char *name) {
    struct passwd *pwd = getpwnam(name); /* don't free, see getpwnam() for details */
    if(pwd == NULL) {
        throw runtime_error(string("Failed to get userId from username : ") + name);
    }
    return pwd->pw_uid;
}

vector<string> split(const char *str, char c = ' ') {
    vector<string> result;

    do {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}

inline string &ltrim(string& s, const char* t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline string &rtrim(string& s, const char* t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1); 
    return s;
}

string &trim(string& s, const char* t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t); 
}

