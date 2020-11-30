#include <iostream>
#include <cstring>
#include <vector>

struct MemoryStruct {
    char *memory;
    size_t size;
};

class haRest {

    private:
        std::string haHostIp = "";
        int haPort = 0;

        bool returnStateOnly = false;   // If true return the state only, on, off etc
                                        // If false return a json fragment e.g.
                                        // { 'state':'off'} 
        bool redis = false;

        std::string haTokenFilename = "/etc/mqtt/haToken.txt";
        std::string haToken = "";

        bool verbose = false;

        std::string toRedis(std::string data);
        void commonInit();

        std::vector<std::string> logicalTrue  = { "on","ON","true","TRUE","yes","YES" };
        std::vector<std::string> logicalFalse = { "off","OFF","false","FALSE","no","NO" };

        bool isOn(std::string);
        bool isOff(std::string);


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

        struct WriteThis {
            const char *readptr;
            size_t sizeleft;
        };
        static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp)
        {
            struct WriteThis *wt = (struct WriteThis *)userp;
            size_t buffer_size = size*nmemb;

            if(wt->sizeleft) {
                /* copy as much as possible from the source to the destination */ 
                size_t copy_this_much = wt->sizeleft;
                if(copy_this_much > buffer_size)
                    copy_this_much = buffer_size;
                memcpy(dest, wt->readptr, copy_this_much);

                wt->readptr += copy_this_much;
                wt->sizeleft -= copy_this_much;
                return copy_this_much; /* we copied this many bytes */ 
            }

            return 0; /* no more data left to deliver */ 
        }

    public:
        haRest();
        haRest(std::string haHost);
        haRest(std::string haHost, int port);

        void setHaHost(std::string ip);
        void setHaPort(int p);

        std::string get(std::string entityId);
        bool set(std::string entityId, std::string state);

        void setReturnStateOnly(bool state);

        void setVerbose(bool state);
        void setRedis(bool state);

        void dump();
};

