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

//#define USE_FORK

static pid_t(*orig_getpid)() = NULL;
static int server_socket = 0;

void dostuff(int);
void memserver_start();
void memserver_accept();

#ifdef USE_FORK
static void
memserver_child_handler(int sig)
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}
#endif

pid_t getpid(){
    if(!server_socket){
        orig_getpid = dlsym(RTLD_NEXT, "getpid");
        printf("[.] (pid=%d) starting server...\n",orig_getpid());
        memserver_start();
    }
    memserver_accept();
    return orig_getpid();
}

void memserver_start(){
    int portno, x;
    struct sockaddr_in serv_addr;

#ifdef USE_FORK
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = memserver_child_handler;
    sigaction(SIGCHLD, &sa, NULL);
#endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
        perror("ERROR creating socket");
        return;
    }

    // make server socket non-blocking
    x=fcntl(server_socket,F_GETFL,0);
    fcntl(server_socket,F_SETFL,x | O_NONBLOCK);

    x = 1;
    setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x) );

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5585;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR on binding");
        return;
    }
    listen(server_socket,5);
}

void memserver_accept(){
     struct sockaddr_in cli_addr;
     socklen_t clilen;
     int client_socket;
     pid_t pid;
     int x;
     struct timeval tv;

     clilen = sizeof(cli_addr);
     while (1) {
         client_socket = accept(server_socket, (struct sockaddr *) &cli_addr, &clilen);
         if (client_socket < 0){
             if( errno == EWOULDBLOCK ) return;
             perror("ERROR on accept");
             return;
         }

         // make client socket blocking
         x=fcntl(client_socket,F_GETFL,0);
         fcntl(client_socket,F_SETFL,x & (~O_NONBLOCK));

         tv.tv_sec = 10;
         tv.tv_usec = 0;
         setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,&tv, sizeof(tv));
         setsockopt(client_socket,SOL_SOCKET,SO_SNDTIMEO,&tv, sizeof(tv));

#ifdef USE_FORK
         pid = fork();
         if (pid < 0) perror("ERROR on fork");
         if (pid == 0)  {
             close(server_socket);


             dostuff(client_socket);
             close(client_socket);
             exit(0);
         }
#else
         dostuff(client_socket);
#endif
         close(client_socket);
     } /* end of while */
}

#define CMD_READ_MEM 0xf8u

void dostuff (int sock)
{
   void *ptr = NULL;
   size_t size = 0;
   int n;
   unsigned int cmd;
      
   while(1){
        cmd = 0;
        n = read(sock,&cmd,1);
        if (n != 1){
            perror("ERROR reading cmd");
            return;
        }

        switch(cmd){
            case CMD_READ_MEM:
                n = read(sock,&ptr,4);
                if (n != 4){
                    perror("ERROR reading ptr");
                    return;
                }
                n = read(sock,&size,4);
                if (n != 4){
                    perror("ERROR reading size");
                    return;
                }

                n = write(sock, ptr, size);
                if(n<=0){
                    shutdown(sock,SHUT_RDWR);
                    fprintf(stderr, "ERROR reading %8lx bytes at %8p: %2d ", size, ptr, errno);
                    perror("");
                    return;
                }
                break;

            default:
                fprintf(stderr, "unknown cmd %x\n", cmd);
                return;
                break;
        }
   }
}
