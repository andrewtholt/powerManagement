#include <string>
#include <stdint.h>

#define BUFFER_SIZE 255

using namespace std;

class myClient{
public:
   static myClient* Instance();
   bool setupNetwork(char *hostName, char *serviceName);

   int getMessage(char *buffer, int32_t len) ; // get message into buffer upto len bytes
   int sendMessage(char *buffer, int32_t len);  // Send message in buffer.

   bool loadFile(string fileName);
   bool clientConnect();
   void clientFlush(); // Read and discard input from server
   bool clientConnected();
   bool sendCmd(string cmd) ;
   bool sendCmd(string cmd, string & res) ;

private:
   int sockfd=-1;
   bool verbose=true;
   bool connected=false;
   myClient(){};  // Private so that it can  not be called
   myClient(myClient const&){};             // copy constructor is private
   myClient& operator=(myClient const&){};  // assignment operator is private
   static myClient* m_pInstance;
};
