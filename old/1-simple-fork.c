#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char*argv[]){
    pid_t pid;

    signal(SIGCHLD, SIG_IGN);

    printf("[.] my pid = %d\n", getpid());

    while(1){
        pid = fork();

        if(pid == 0){
            printf("[.] I'm a Child! my pid = %d\n", getpid());
            exit(0);
        } else {
            printf("[.] I'm a Parent! my pid = %d\n", getpid());
            sleep(5);
        }
    }

    return 0;
}
