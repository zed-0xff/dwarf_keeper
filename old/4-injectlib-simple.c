#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static pid_t(*orig_getpid)() = NULL;

pid_t getpid(){
    int n;

    printf("[!!!] Yay!! I'm a fake getpid() ^____^ (stack: %p)\n", &n);
    orig_getpid = dlsym(RTLD_NEXT, "getpid");
    return orig_getpid();
}
