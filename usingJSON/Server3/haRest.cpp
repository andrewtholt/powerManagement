/***********************************************************************
 * AUTHOR: andrewh <andrewh>
 *   FILE: .//haRest.cpp
 *   DATE: Thu Nov 26 14:11:56 2020
 *  DESCR: 
 ***********************************************************************/
#include "haRest.h"
#include <fstream>

#include <curl/curl.h>



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

    url = "http://" + haHostIp + ":" + std::to_string(port) + "/api/states/" + entityId;

    CURLcode ret;    
    CURL *hnd;    
    struct curl_slist *slist1;    

    curl_global_init(CURL_GLOBAL_ALL);

    slist1 = NULL;    
    slist1 = curl_slist_append(slist1, "Authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiIxZjJlZGVmYWJiYTY0ZjAxOTc4YmM4YWI3ZWFmZjQzZSIsImlhdCI6MTU2ODY0OTExMiwiZXhwIjoxODg0MDA5MTEyfQ.XLFUZQD6BJAQpydjoSkvke8vhpQZ40mAS6YyAnISNOQ");             
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");    

    hnd = curl_easy_init();    
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);    
//  curl_easy_setopt(hnd, CURLOPT_URL, "http://192.168.10.124:8123/api/states/switch.test_start");                                                    
    printf("URL >%s<\n", url.c_str());
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

    std::cout << "Ret code " << ret << std::endl;

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


