#include "common.h"

class OffscreenRenderer : MemClass {
    char buf[0x1000];

    public:

    OffscreenRenderer(int width, int height){
        memset(buf,0,sizeof(buf));
        if(GAME.offscr_renderer_ctor_func){
            ((func_t_pii)GAME.offscr_renderer_ctor_func)(this, width, height);
        }
    }

    void render(int x0, int y0){
        if(GAME.offscr_renderer_render_func){
            ((func_t_pii)GAME.offscr_renderer_render_func)(this, x0, y0);
        }
    }

    bool save(void*bmp_buf, int bufsize){
        static void*p1 = NULL; 
        static void*p2 = NULL; 

        if(!bmp_buf) return false;

        if(!p1) p1 = dlsym(RTLD_DEFAULT, "SDL_RWFromMem");
        if(!p2) p2 = dlsym(RTLD_DEFAULT, "SDL_SaveBMP_RW");

        if( p1 && p2 ){
            // create in-memory RW
            void *rw = (void*)((func_t_pi)p1)(bmp_buf, bufsize);

            // save bmp to a newly created RW
            if( 0 == ((func_t_ppi)p2)(getSurface(), rw, 1)){
                return true;
            }
        } else {
            printf("[!] p1=%p, p2=%p\n", p1, p2);
        }
        return false;
    }

    SDL_Surface* getSurface(){
        return (SDL_Surface*)dw(SURFACE_OFFSET);
    }

    ~OffscreenRenderer(){
        if(GAME.offscr_renderer_dtor_func){
            ((func_t_p)GAME.offscr_renderer_dtor_func)(this);
        }
    }

    //////////////////////////////////////////////////////////////////

    private:
    // offset in instance data
    static const int SURFACE_OFFSET         = 0x34; // SDL_Surface
};
