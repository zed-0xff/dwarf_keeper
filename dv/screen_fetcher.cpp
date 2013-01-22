#include "fetcher.cpp"

class ScreenFetcher {

    Fetcher fetcher;
    RemoteScreen screen;

    void loop(){
        size_t dl;

        // add empty HTTP headers to prevent curl sending them at all
        // this saves some traffic, especially when requests are made >10 times per second
        fetcher.add_header("Accept:");
        fetcher.add_header("Host:");

        while( 1 ){
            if( dl = fetcher.fetch_screen(screen) ){
                pthread_mutex_lock(&g_remote_screen_mutex);
                SDL_Event ev;
                ev.type = SDL_USEREVENT;
                SDL_PushEvent(&ev);
                g_remote_screen = screen;
                g_remote_screen.dl_size = dl;
                pthread_mutex_unlock(&g_remote_screen_mutex);
            } else {
                SDL_Delay(50);
            }
        }
    }

    public:

    static void start_thread(){
        SDL_CreateThread(thread_start, NULL);
    }

    private:

    static int thread_start(void*){
        ScreenFetcher instance;
        instance.loop();
    }
};
