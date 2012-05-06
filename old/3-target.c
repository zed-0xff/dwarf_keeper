#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char*argv[]){
    pid_t pid;
    char buf[0x200],*p;

    strcpy(buf,"SECRET");
    strcat(buf,"=");
    strcat(buf,"F");
    strcat(buf,"0");
    strcat(buf,"0");
    strcat(buf,"B");
    strcat(buf,"A");
    strcat(buf,"R");
    strcat(buf,"!");

    p = buf+strlen(buf);

    printf("[.] going to infinite loop...\n");
    while(1){
        sprintf(p,"%d",rand());
        printf("[.] my pid = %d\n",getpid());
        sleep(1);
    }

    return 0;
}
