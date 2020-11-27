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
haRest::haRest()
{
}

/***********************************************************************
 *  Method: haRest::haRest
 *  Params: std::string haHost
 * Effects: 
 ***********************************************************************/
haRest::haRest(std::string haHost) {
    haHostIp = haHost;
}

/***********************************************************************
 *  Method: haRest::haRest
 *  Params: std::string haHost, int port
 * Effects: 
 ***********************************************************************/
haRest::haRest(std::string haHost, int p) {

    commonInit();
    haHostIp = haHost;
    port = p;
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

    url = "http://" + haHostIp + ":" + std::to_string(port) + "/api/states/" + entityId;

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
    //  curl_easy_setopt(hnd, CURLOPT_URL, "http://192.168.10.124:8123/api/states/switch.test_start");                                                    
    printf("URL >%s<\n", url.c_str());
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
        printf("\n%s\n", chunk.memory);
    }

    if( returnStateOnly == true ) {
        json jsonMsg = json::parse( chunk.memory );

        result = jsonMsg["state"];
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
std::string haRest::set(std::string entityId, std::string state) {
    std::string result = "EMPTY";

    return result;
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
    std::cout << "HA Host IP  : " << haHostIp   << std::endl;
    std::cout << "HA Host port: " << port       << std::endl;
    std::cout << "HA Token    : " << haToken    << std::endl;
}


/***********************************************************************
 *  Method: haRest::setHaPort
 *  Params: int p
 * Returns: void
 * Effects: 
 ***********************************************************************/
void haRest::setHaPort(int p) {
    if( port == 0) {
        port = p;
    }
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


