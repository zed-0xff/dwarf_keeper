#include <SDL/SDL.h>

class LocalScreen {
    int w, h;
    int flags;

    public:

    SDL_Surface* surface;

    LocalScreen(){
        w = 0;
        h = 0;
        flags = SDL_RESIZABLE | SDL_DOUBLEBUF;
        surface = NULL;
    }

    void resize(int neww, int newh){
        w = neww;
        h = newh;
        surface = SDL_SetVideoMode( w, h, 0, flags );
    }
};
