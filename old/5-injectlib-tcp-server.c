#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <fcntl.h>

static pid_t(*orig_getpid)() = NULL;
static int server_socket = 0;

void dostuff(int);
void server_start();
void server_accept();

pid_t getpid(){
    if(!server_socket){
        printf("[.] starting server...\n");
        server_start();
    }
    server_accept();
    orig_getpid = dlsym(RTLD_NEXT, "getpid");
    return orig_getpid();
}

void error(const char *msg)
{
    perror(msg);
}

void server_accept(){
     struct sockaddr_in cli_addr;
     socklen_t clilen;
     int newsockfd;
     pid_t pid;

     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(server_socket, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0){
             if( errno == EWOULDBLOCK ) return;
             error("ERROR on accept");
         }
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(server_socket);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
}

void server_start(){
     int portno, x;
     struct sockaddr_in serv_addr, cli_addr;

     server_socket = socket(AF_INET, SOCK_STREAM, 0);
     if (server_socket < 0) 
        error("ERROR opening socket");

     x=fcntl(server_socket,F_GETFL,0);
     fcntl(server_socket,F_SETFL,x | O_NONBLOCK);

     x = 1;
     setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x) );

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 5555;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(server_socket, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(server_socket,5);
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
      
   //bzero(buffer,256);
   //n = read(sock,buffer,255);
   //if (n < 0) error("ERROR reading from socket");
   //printf("Here is the message: %s\n",buffer);
   n = write(sock,((void*)&n),0x2000);
   if (n < 0) error("ERROR writing to socket");
   close(sock);
}
