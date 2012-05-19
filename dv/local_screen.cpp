#include <SDL/SDL.h>

class LocalScreen {
    int w, h;
    int qw, qh; // queued resize
    int flags;

    public:

    pthread_mutex_t resize_mutex;
    SDL_Surface* surface;

    LocalScreen(){
        w = qw = 0;
        h = qh = 0;
        flags = SDL_RESIZABLE|SDL_DOUBLEBUF;
        pthread_mutex_init(&resize_mutex, NULL);
        surface = NULL;
    }

    void resize(int neww, int newh){
        pthread_mutex_lock(&resize_mutex);
        w = qw = neww;
        h = qh = newh;
        surface = SDL_SetVideoMode( w, h, 0, flags );
        pthread_mutex_unlock(&resize_mutex);
    }

    void queue_resize(int neww, int newh){
        pthread_mutex_lock(&resize_mutex);
        qw = neww;
        qh = newh;
        pthread_mutex_unlock(&resize_mutex);
    }

    void resize_if_needed(){
        pthread_mutex_lock(&resize_mutex);
        if( qw > 0 && qh > 0 && (qw != w || qh != h)){
            resize(qw, qh);
        }
        pthread_mutex_unlock(&resize_mutex);
    }
};
