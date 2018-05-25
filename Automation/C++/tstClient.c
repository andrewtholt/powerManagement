#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct hostent *server;
   
   char buffer[256];
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
	
   struct addrinfo *result;
   struct addrinfo *rp;

   int rc = getaddrinfo( argv[1], argv[2], NULL, &result);

   if( rc != 0 ) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
       exit(1);
   }

   for( rp = result; rp != NULL; rp = rp->ai_next) {
       sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

       if( sockfd == -1) {
           continue;
       }

       if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
           break;
       }

       close(sockfd);
    }
   
   /* Read server message */
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   /* Now ask for a message from the user, this message
      * will be read by server
   */
	
   printf("Please enter the message: ");
   bzero(buffer,256);
   fgets(buffer,255,stdin);
   
   /* Send message to the server */
   n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
	
   printf("%s\n",buffer);
   return 0;
}
