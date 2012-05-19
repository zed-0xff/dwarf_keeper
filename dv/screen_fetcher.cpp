#include "fetcher.cpp"

class ScreenFetcher {

    Fetcher fetcher;
    Screen screen;

    void loop(){
        while( 1 ){
            if(fetcher.fetch_screen(screen)){
                // lock mutex
                // copy screen
                // unlock mutex
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
