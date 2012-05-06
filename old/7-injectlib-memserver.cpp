#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>

#include <string>

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
    printf("[*] gathering zombies...\n");
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}
#endif

pid_t getpid(){
    if(!server_socket){
        orig_getpid = (pid_t (*)()) dlsym(RTLD_NEXT, "getpid");
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
    // XXX linux-only?
//    struct sigaction sa;
//    sigemptyset(&sa.sa_mask);
//    sa.sa_flags = 0;
//    sa.sa_handler = memserver_child_handler;
//    sigaction(SIGCHLD, &sa, NULL);

//    XXX bsd*-only?
    signal(SIGCHLD, SIG_IGN);
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
         if (pid == 0) {
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

#define CMD_READ_MEM    0xf8
#define CMD_ITEM_INFO   0xf9
#define CMD_DWARF_NAME  0xfa
#define CMD_DWARVES     0xfb
#define CMD_ITEMS       0xfc

#define CREATURE_FULL_NAME_FUNC 0x96b030
#define ITEM_INFO_FUNC          0x612c10

typedef int(*info_func_t)(void*, std::string*, int);

info_func_t getCreatureFullName = (info_func_t)CREATURE_FULL_NAME_FUNC;
info_func_t getItemName         = (info_func_t)ITEM_INFO_FUNC;

void item_info(int sock){
    int n;
    void *pvtable = NULL;
    std::string s;

    n = read(sock,&pvtable,4);
    if (n != 4){
        perror("ERROR reading ptr");
        return;
    }

    getItemName(pvtable, &s, 0);

    n = s.length();
    write(sock, &n, 4);
    write(sock, s.data(), n);
}


void dwarf_name(int sock){
    int n;
    void *pcreat = NULL;
    info_func_t func = (info_func_t)CREATURE_FULL_NAME_FUNC;
    std::string s;

    n = read(sock,&pcreat,4);
    if (n != 4){
        perror("ERROR reading ptr");
        return;
    }

    n = func(pcreat, &s, 0);
    printf("[.] dwarf_name_func returned %x\n", n);

    n = s.length();
    write(sock, &n, 4);
    write(sock, s.data(), n);
}

#define CREATURES_VECTOR    0x157e5f8
#define ITEMS_VECTOR        0x157e668

std::string wears2string(void**vector){
    void **vend;
    std::string rs;

    //printf("[d] items v = %p\n", vector);

    vend   = *(void***)(vector+1);
    vector = *(void***)vector;
    if(!vector) return rs;

    //printf("[d] items v = %p\n", vector);
    
    printf("[d] items v = %p, %p\n", vector, vend);

    if(vector && vend && vector < vend){
        while(vector < vend){
            void* pwear = *vector;
            if(!pwear) break;

            void* pitem = *(void**)pwear;
            if(!pitem) break;

            std::string s;
            printf("[d] vector %p: item %p\n", vector, pitem);
            getItemName(pitem, &s, 0);
            printf("[d] item name %s\n", s.data());
            rs += "\t" + s + "\n";
            vector++;
        }
    }

    return rs;
}

void creatures(int sock){
    uint32_t *pcreat = NULL;
    void **vector, **vend;
    std::string sAll,s;
    int n;

    vector = *(void***)CREATURES_VECTOR;
    vend   = *(void***)(CREATURES_VECTOR+4);
    
    while(vector < vend){
        void* pcreat = *vector;
        if(!pcreat) break;
        getCreatureFullName(pcreat, &s, 0);
        sAll += s + "\n";
        printf("[d] pcreat = %p\n", pcreat);
        sAll += wears2string((void**)((char*)pcreat + 0x22c));
        vector++;
    }

    n = sAll.length();
    write(sock, &n, 4);
    write(sock, sAll.data(), n);
}

void items(int sock){
    void **vector, **vend;
    std::string rs;

    vector = *(void***)ITEMS_VECTOR;
    vend   = *(void***)(ITEMS_VECTOR+4);

    if(vector && vend && vector < vend){
        while(vector < vend){
            void* pitem = *vector;
            if(!pitem) break;

            std::string s;
            printf("[d] vector %p: item %p\n", vector, pitem);
            getItemName(pitem, &s, 0);
            printf("[d] item name %s\n", s.data());
            rs += "\t" + s + "\n";
            vector++;
        }
    }

    int n = rs.length();
    write(sock, &n, 4);
    write(sock, rs.data(), n);
}

void dostuff(int sock)
{
   void *ptr = NULL;
   size_t size = 0;
   int n;
   unsigned int cmd;
      
   while(1){
        cmd = 0;
        n = read(sock,&cmd,1);
        if (n == 0) return; // EOF / socket closed by client
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

            case CMD_ITEM_INFO:
                item_info(sock);
                break;

            case CMD_DWARF_NAME:
                dwarf_name(sock);
                break;

            case CMD_DWARVES:
                creatures(sock);
                break;

            case CMD_ITEMS:
                items(sock);
                break;

            default:
                fprintf(stderr, "unknown cmd %x\n", cmd);
                return;
                break;
        }
   }
}
