#include <SDL/SDL.h>
#include <SDL_picofont.h>
#ifdef __osx__
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif
#include "fetcher.cpp"
#include "event_sender.cpp"

using namespace std;

#ifdef __osx__
using namespace tr1;
#endif

class Drawer {

    unordered_map <uint16_t, SDL_Surface*> tilecache;

    int tile_width, tile_height;
    SDL_Rect tileRect;
    SDL_Surface *default_tile;

    int ping_time[4], draw_time[4];
    int steps;
    struct timeval t_start;

    Fetcher fetcher;
    EventSender event_sender;
    public:

    Drawer(){
        default_tile = g_screen.surface = NULL;
        memset(ping_time, 0, sizeof(ping_time));
        memset(draw_time, 0, sizeof(draw_time));
        steps = 0;
        gettimeofday(&t_start, NULL);
    }

    void resize_tile(int w, int h){
        if( w < 1) w = 1;
        if( h < 1) h = 1;

        // force square tiles
        w = h = min(w,h);

        if( w == tile_width && h == tile_height ) return;

        if(default_tile){
            SDL_FreeSurface(default_tile);
            default_tile = NULL;
        }
        default_tile = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 32,0,0,0,0);

        tileRect.w = w; tileRect.h = h;
        tile_width = w; tile_height = h;

        if( g_screen.surface ) SDL_FillRect(g_screen.surface, NULL, 0);
    }

    void check_sizes(){
        int rpxw = g_remote_screen.width*tile_width;
        int lpxw = g_screen.surface->w;
        int rpxh = g_remote_screen.height*tile_height;
        int lpxh = g_screen.surface->h;

        // remote screen width is bigger than local at least for tile_width/2 pixels
        if( rpxw - lpxw >= tile_width/2 ){
            resize_tile(lpxw/g_remote_screen.width, lpxh/g_remote_screen.height);
        }

        // remote width is smaller
        if( lpxw - rpxw >= lpxw/tile_width ){
            resize_tile(lpxw/g_remote_screen.width, lpxh/g_remote_screen.height);
        }

        // remote screen height is bigger than local at least for tile_height/2 pixels
        if( rpxh - lpxh >= tile_height/2 ){
            resize_tile(lpxh/g_remote_screen.height, lpxh/g_remote_screen.height);
        }

        // remote height is smaller
        if( lpxh - rpxh >= lpxh/tile_height ){
            resize_tile(lpxh/g_remote_screen.height, lpxh/g_remote_screen.height);
        }

    }

    void draw_info(){
        char buf[0x200];

        if( draw_time[0] > draw_time[2]) draw_time[2] = draw_time[0];
        if( ping_time[0] > ping_time[2]) ping_time[2] = ping_time[0];

        draw_time[3] += draw_time[0];
        ping_time[3] += ping_time[0];
        steps += 1;
        ping_time[1] = ping_time[3] / steps;
        draw_time[1] = draw_time[3] / steps;

        struct timeval t1;
        gettimeofday(&t1, NULL);
        int ms = diff_ms(t1, t_start);

        sprintf(buf, 
                "tiles: %ld\n"
                "ping:  %4dms %4dms %4dms\n"
                "draw:  %4dms %4dms %4dms\n"
#ifdef __osx__
                "dl:   %5lldk %5lldKbit/s",
#else
                "dl:   %5ldk %5ldKbit/s",
#endif
                tilecache.size(),
                ping_time[0], ping_time[1], ping_time[2],
                draw_time[0], draw_time[1], draw_time[2],
                fetcher.total_dl / 1024,
                fetcher.total_dl / ms * 8 * 1000 / 1024
                );
        SDL_Surface* text = FNT_Render(buf, (SDL_Color){0xff,0xff,0xff});
        SDL_Rect r = {0, 0, (Uint16)text->w, (Uint16)text->h};
        SDL_FillRect(g_screen.surface, &r, 0);
        SDL_BlitSurface(text, NULL, g_screen.surface, NULL);
        SDL_FreeSurface(text);
    }

    void loop(){
        SDL_Event event;
        vector<SDL_Event> events_queue;
        struct timeval t0,t1;
        int gameover = 0;
        uint32_t remote_last_hash = 0;
        bool remote_screen_ready = false;

        fetcher.fetch_screen(g_remote_screen);
        resize_tile(g_remote_screen.tile_width, g_remote_screen.tile_height);

        g_screen.resize( g_remote_screen.pixelWidth(), g_remote_screen.pixelHeight() );

        if(!g_screen.surface){
            error("SDL_SetVideoMode fail");
        }

        while(!gameover){
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
                  g_screen.resize( event.resize.w, event.resize.h );
                  events_queue.push_back(event);
                  break;

                case SDL_USEREVENT:
                  remote_screen_ready = true;
                  break;

                default:
                  events_queue.push_back(event);
                  break;
              }

            }

            if( !events_queue.empty() ){
                gettimeofday(&t0, NULL);

                fetcher.post_events(events_queue);

                gettimeofday(&t1, NULL);
                ping_time[0] = diff_ms(t1,t0);
            }

            if(gameover) break;

            if( remote_screen_ready ){
                pthread_mutex_lock(&g_remote_screen_mutex);
                if( g_remote_screen.hash != remote_last_hash ){
                    gettimeofday(&t0, NULL);

                    fetcher.total_dl += g_remote_screen.dl_size;

                    //printf("[d] calling draw %x %x\n", g_remote_screen.hash, remote_last_hash);
                    draw();

                    gettimeofday(&t1, NULL);
                    draw_time[0] = diff_ms(t1,t0);

                    remote_last_hash = g_remote_screen.hash;
                }
                remote_screen_ready = false;
                pthread_mutex_unlock(&g_remote_screen_mutex);
            }

            draw_info();
            SDL_Flip(g_screen.surface);
            SDL_Delay(10);
        }

        /* clean up */
        //SDL_FreeSurface(sprite);
        SDL_Quit();
    }

    private:

    SDL_Surface* fetch_tile(uint16_t tile_id){
        string *ps = fetcher.fetch_tile(tile_id);
        if(!ps) return NULL;

        SDL_RWops* rw    = SDL_RWFromConstMem(ps->data(), ps->size());
        if(!rw) return NULL;

        SDL_Surface* tmp = SDL_LoadBMP_RW(rw, 1); // auto frees rw
        if(!tmp) return NULL;

        SDL_Surface* tile = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);

        tilecache[tile_id] = tile;
        return tile;
    }

    void draw(){
        check_sizes();

        for(int y=0; y<g_remote_screen.height; y++){
            for(int x=0; x<g_remote_screen.width; x++){
                uint16_t tile_id = g_remote_screen.at(x,y);
                SDL_Surface*tile = tilecache[tile_id];

                if( !tile ){
                    tile = fetch_tile(tile_id);
                    if( !tile ){
                        // fetch failed, use default tile
                        tile = default_tile;
                        tilecache[tile_id] = tile;
                    }
                }
                
                tileRect.x = x*tile_width;
                tileRect.y = y*tile_height;
                //printf("[d] blit %x at (%d, %d)\n", tile_id, x,y);
                SDL_BlitSurface(tile, NULL, g_screen.surface, &tileRect);
            }
        }
    }
};
