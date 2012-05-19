#include "fetcher.cpp"

class ScreenFetcher {

    Fetcher fetcher;
    RemoteScreen screen;

    void loop(){
        while( 1 ){
            if(fetcher.fetch_screen(screen)){
                printf("[d] screen fetched\n");
                pthread_mutex_lock(&g_remote_screen_mutex);
                SDL_Event ev;
                ev.type = SDL_USEREVENT;
                SDL_PushEvent(&ev);
                g_remote_screen = screen;
                pthread_mutex_unlock(&g_remote_screen_mutex);
            } else {
                SDL_Delay(10);
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
