#include <stdio.h>
#include <SDL/SDL.h>
#include <pthread.h>

void error(const char *msg) {
    SDL_Quit();
    perror(msg);
    exit(1);
}

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

#include "local_screen.cpp"

LocalScreen g_screen;

#include "screen.cpp"
#include "fetcher.cpp"
#include "drawer.cpp"
#include "event_sender.cpp"

int main ( int argc, char *argv[] ){
    if( argc > 1 ){
        Fetcher::g_host = argv[1];
        printf("[.] connecting to %s...\n", argv[1]);
    } else {
        Fetcher::g_host = "localhost";
        printf("[.] no address given, using localhost...\n");
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_EnableUNICODE(1);
    SDL_WM_SetCaption("SDL", "SDL");

    g_screen.resize(640, 480);

    EventSender::start_thread();

    Drawer d;
    d.draw();

    return 0;
}
