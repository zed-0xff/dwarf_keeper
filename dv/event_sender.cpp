#include <pthread.h>
#include <vector>
#include "fetcher.cpp"

using namespace std;

class EventSender {

    Fetcher fetcher;

    void loop(){
        vector <SDL_Event> events_queue;
        SDL_Event event;
        int gameover = 0;

        while( !gameover && SDL_WaitEvent(NULL) ){
          events_queue.clear();
          while (SDL_PollEvent(&event)) {
            switch (event.type) {
              /* close button clicked */
              case SDL_QUIT:
                gameover = 1;
                events_queue.push_back(event);
                break;

              case SDL_VIDEORESIZE:
                printf("[d] resize %dx%d\n", event.resize.w, event.resize.h);
                g_screen.queue_resize( event.resize.w, event.resize.h);
                events_queue.push_back(event);
                break;

              default:
                events_queue.push_back(event);
                break;
            }

          }

          if( !events_queue.empty() ){
              fetcher.post_events(events_queue);
          }
        }

        SDL_Quit();
        exit(0);
    }

    public:

    static void start_thread(){
        pthread_create(&thread_id, NULL, thread_start, NULL);
    }

    private:

    static pthread_t thread_id;

    static void* thread_start(void*){
        EventSender es;
        es.loop();
    }
};

pthread_t EventSender::thread_id;
