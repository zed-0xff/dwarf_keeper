#ifndef EVENT_SENDER_H
#define EVENT_SENDER_H

#include <pthread.h>
#include <vector>
#include "fetcher.cpp"

using namespace std;

class EventSender {

    Fetcher fetcher;
    int gameover;

    void loop(){
        gameover = 0;

        while( !gameover && SDL_WaitEvent(NULL) ){
            send_events();
        }

        SDL_Quit();
        exit(0);
    }

    public:

    void send_events(){
        SDL_Event event;
        vector <SDL_Event> events_queue;

        while (SDL_PollEvent(&event)) {
          switch (event.type) {
            /* close button clicked */
            case SDL_QUIT:
              gameover = 1;
              events_queue.push_back(event);
              break;

            case SDL_VIDEORESIZE:
              printf("[d] resize %dx%d\n", event.resize.w, event.resize.h);
              //g_screen.queue_resize( event.resize.w, event.resize.h);
              events_queue.push_back(event);
              break;

            case SDL_USEREVENT:
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

    static void start_thread(){
        //pthread_create(&thread_id, NULL, thread_start, NULL);
        SDL_CreateThread(thread_start, NULL);
    }

    private:

    static pthread_t thread_id;

    static int thread_start(void*){
        EventSender es;
        es.loop();
    }
};

pthread_t EventSender::thread_id;

#endif
