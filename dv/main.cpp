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
#include "remote_screen.cpp"

LocalScreen     g_screen;
RemoteScreen    g_remote_screen;
pthread_mutex_t g_remote_screen_mutex;
int             g_desktop_w=0, g_desktop_h=0; // local desktop size

#include "fetcher.cpp"
#include "drawer.cpp"
#include "event_sender.cpp"
#include "screen_fetcher.cpp"

int main ( int argc, char *argv[] ){
    if( argc > 1 ){
        Fetcher::g_host = argv[1];
        printf("[.] connecting to %s...\n", argv[1]);
    } else {
        Fetcher::g_host = "localhost";
        printf("[.] no address given, using localhost...\n");
    }

    pthread_mutex_init(&g_remote_screen_mutex, NULL);

    SDL_Init(SDL_INIT_VIDEO);
    
    if( const SDL_VideoInfo* vinfo = SDL_GetVideoInfo() ){
        g_desktop_w = vinfo->current_w;
        g_desktop_h = vinfo->current_h;
    }

    SDL_EnableUNICODE(1);
    SDL_WM_SetCaption("SDL", "SDL");

    g_screen.resize(640, 480);

    //EventSender::start_thread();
    ScreenFetcher::start_thread();

    Drawer d;
    d.loop();

    return 0;
}
