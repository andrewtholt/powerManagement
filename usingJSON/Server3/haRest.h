#include <iostream>
#include <cstring>

struct MemoryStruct {
    char *memory;
    size_t size;
};

class haRest {

    private:
        std::string haHostIp = "";
        int port = 0;

        bool returnStateOnly = true;   // If true return the state only, on, off etc
                                        // if false return the entire dtat received 

        std::string haTokenFilename = "/etc/mqtt/haToken.txt";
        std::string haToken = "";

        void commonInit();

        static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
            size_t realsize = size * nmemb;    
            struct MemoryStruct *mem = (struct MemoryStruct *)userp;    

            char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);    
            if(ptr == NULL) {    
                // out of memory!        
                printf("not enough memory (realloc returned NULL)\n");    
                return 0;    
            }    

            mem->memory = ptr;    
            memcpy(&(mem->memory[mem->size]), contents, realsize);    
            mem->size += realsize;    
            mem->memory[mem->size] = 0;    

            return realsize;
        };

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

