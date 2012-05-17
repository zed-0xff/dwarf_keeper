#include <stdio.h>

#include "screen.cpp"
#include "fetcher.cpp"
#include "drawer.cpp"

static void error(const char *msg) {
    SDL_Quit();
    perror(msg);
    exit(1);
}

int main ( int argc, char *argv[] ){
    Drawer d;

    d.draw();

    return 0;
}
