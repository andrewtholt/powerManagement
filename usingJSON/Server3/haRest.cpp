/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: .//haRest.cpp
 *   DATE: Thu Nov 26 14:11:56 2020
 *  DESCR: 
 ***********************************************************************/
#include "haRest.h"
#include <fstream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/***********************************************************************
 *  Method: haRest::haRest
 *  Params: 
 * Effects: 
 ***********************************************************************/
haRest::haRest() {
    haHostIp = "127.0.0.1";
    haPort = 6379;

}

/***********************************************************************
 *  Method: haRest::haRest
 *  Params: std::string haHost
 * Effects: 
 ***********************************************************************/
haRest::haRest(std::string haHost) {
    haHostIp = haHost;
    haPort = 6379;
}

/***********************************************************************
 *  Method: haRest::haRest
 *  Params: std::string haHost, int p
 * Effects: 
 ***********************************************************************/
haRest::haRest(std::string haHost, int p) {

    commonInit();
    haHostIp = haHost;
    haPort = p;
}

/***********************************************************************
 *  Method: haRest::setHaPort
 *  Params: int p
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::setHaPort(int p) {
    if( haPort == 0) {
        haPort = p;
    }
}


/***********************************************************************
 *  Method: haRest::get
 *  Params: std::string entityId
 * Returns: std::string
 * Effects: 
 ***********************************************************************/
std::string haRest::get(std::string entityId) {
    std::string result = "EMPTY";
    std::string url;
    std::string tmp;
    struct MemoryStruct chunk;

    chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */

    url = "http://" + haHostIp + ":" + std::to_string(haPort) + "/api/states/" + entityId;

    CURLcode ret;    
    CURL *hnd;    
    struct curl_slist *slist1;    

    curl_global_init(CURL_GLOBAL_ALL);

    slist1 = NULL;    

    std::string bearer = "Authorization: Bearer " + haToken;

    slist1 = curl_slist_append(slist1, bearer.c_str());
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");    

    hnd = curl_easy_init();    
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);    

    if (verbose) {
        std::cout << "URL >" + url + "<\n";
    }
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);        
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);    
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.73.0");    
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);    
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);    
    curl_easy_setopt(hnd, CURLOPT_SSH_KNOWNHOSTS, "/home/andrewh/.ssh/known_hosts");    
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");    
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);    

    ret = curl_easy_perform(hnd);

    if( chunk.size > 0) {
        if(verbose) {
            printf("\n%s\n", chunk.memory);
        }
    }

    if( returnStateOnly == true ) {
        json jsonMsg = json::parse( chunk.memory );

        result = jsonMsg["state"];

        if (redis == true ) {
            result = toRedis(result);
        } else {
            result += '\n';
        }

    } else {
        result = chunk.memory;
    }

    std::cout << "Ret code " << ret << std::endl;

    free(chunk.memory);

    curl_easy_cleanup(hnd);    
    hnd = NULL;    
    curl_slist_free_all(slist1);    
    slist1 = NULL;
    return result;
}


/***********************************************************************
 *  Method: haRest::set
 *  Params: std::string entityId, std::string state
 * Returns: std::string
 * Effects: 
 ***********************************************************************/
bool haRest::set(std::string entityId, std::string state) {
    
    bool failFlag = true;

    bool off= isOff(state);
    bool on = isOn(state);
    std::string toState="INVALID";

    if( off == false && on == true ) {
        toState = "turn_on";
    } else if ( off == true && on == false) {
        toState = "turn_off";
    } else {
        std::cout << "set state invalid\n" ;

        return failFlag;
    }

    CURLcode ret;    
    CURL *hnd;    
    struct curl_slist *slist1;    

    std::string url = "http://" + haHostIp + ":" + std::to_string(haPort) + "/api/services/switch/" + toState;

    if (verbose) {
        std::cout << "URL >" + url + "<\n";
    }

    struct WriteThis wt;
 

    curl_global_init(CURL_GLOBAL_ALL);

    slist1 = NULL;    

    std::string bearer = "Authorization: Bearer " + haToken;

    slist1 = curl_slist_append(slist1, bearer.c_str());
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");    


    std::string ei = "{\"entity_id\": \"" + entityId + "\"}";
    if(verbose) {
        std::cout << "Entity Id\n" + ei + "\n" ;
    }

    char eiBuffer[512];
    memset(eiBuffer,0,512);

    strcpy(eiBuffer, ei.c_str());

    wt.readptr = eiBuffer;
    wt.sizeleft = strlen(eiBuffer);

    hnd = curl_easy_init();    
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);    
    curl_easy_setopt(hnd, CURLOPT_URL,url.c_str());
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);    
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, eiBuffer);

//    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)40);    
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, strlen(eiBuffer));
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);    
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.73.0");    
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);    
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");    
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(hnd, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(hnd, CURLOPT_READDATA, &wt);
    curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);

    ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);

    hnd = NULL;
    curl_slist_free_all(slist1);
    slist1 = NULL;
    failFlag = false;

    return failFlag;
}

/***********************************************************************
 *  Method: haRest::setHaHost
 *  Params: std::string ip
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::setHaHost(std::string ip) {
    //
    // can be set only once, either when the class is instantiated, or
    // now.
    //
    if ( haHostIp == "") {
        haHostIp = ip;
    }
}


/***********************************************************************
 *  Method: haRest::dump
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::dump() {
    std::cout << "\n============================\n";
    std::cout << "HA Host IP  : " << haHostIp   << std::endl;
    std::cout << "HA Host port: " << haPort       << std::endl;
    std::cout << "HA Token    : " << haToken    << std::endl;
}



/***********************************************************************
 *  Method: haRest::commonInit
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::commonInit() {
    std::ifstream tokenValue(haTokenFilename);

    std::string line ; 
    std::getline(tokenValue, haToken);


    tokenValue.close();
}


/***********************************************************************
 *  Method: haRest::WriteMemoryCallback
 *  Params: void *contents, size_t size, size_t nmemb, void *userp
 * Returns: size_t
 * Effects: 
 ***********************************************************************/
/*
   static size_t haRest::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {

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

}
*/


/***********************************************************************
 *  Method: haRest::setReturnStateOnly
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::setReturnStateOnly(bool s) {
    returnStateOnly = s;
}

/***********************************************************************
 *  Method: haRest::setVerbose
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::setVerbose(bool s) {
    verbose = s;
}


/***********************************************************************
 *  Method: haRest::isOn
 *  Params: std::string
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool haRest::isOn(std::string inState) {
    bool state = false;

    for(auto & elem : logicalTrue ) {
        if( inState.compare(elem) == 0 ) {
            state = true;
            break;
        }
    }

    return state;
}


/***********************************************************************
 *  Method: haRest::isOff
 *  Params: std::string
 * Returns: bool
 * Effects: 
 ***********************************************************************/
bool haRest::isOff(std::string inState) {
    bool state = false;

    for(auto & elem : logicalFalse ) {
        if( inState.compare(elem) == 0 ) {
            state = true;
            break;
        }
    }

    return state;
}


/***********************************************************************
 *  Method: haRest::toRedis
 *  Params: std::string data
 * Returns: std::string
 * Effects: 
 ***********************************************************************/
std::string haRest::toRedis(std::string data) {
    std::string result = "";

    result = "$";

    result += std::to_string(data.length()) + "\r\n";
    result += data + "\r\n";
    return result;
}


/***********************************************************************
 *  Method: haRest::setRedis
 *  Params: bool state
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::setRedis(bool state) {
    redis = state;
}


