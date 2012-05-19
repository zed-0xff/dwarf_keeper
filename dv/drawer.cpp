#include <SDL/SDL.h>
#include <map>
#include "fetcher.cpp"
#include <SDL_picofont.h>

using namespace std;

class Drawer {

    map <uint32_t, SDL_Surface*> tilecache;
    Screen remote_screen;

    int tile_width, tile_height;
    SDL_Rect tileRect;
    SDL_Surface *default_tile;

    int ping_time[4], draw_time[4];
    int steps;
    struct timeval t_start;

    public:
    Fetcher scr_fetcher;

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
        int rpxw = remote_screen.width*tile_width;
        int lpxw = g_screen.surface->w;
        int rpxh = remote_screen.height*tile_height;
        int lpxh = g_screen.surface->h;

        // remote screen width is bigger than local at least for tile_width/2 pixels
        if( rpxw - lpxw >= tile_width/2 ){
            resize_tile(lpxw/remote_screen.width, lpxh/remote_screen.height);
        }

        // remote width is smaller
        if( lpxw - rpxw >= lpxw/tile_width ){
            resize_tile(lpxw/remote_screen.width, lpxh/remote_screen.height);
        }

        // remote screen height is bigger than local at least for tile_height/2 pixels
        if( rpxh - lpxh >= tile_height/2 ){
            resize_tile(lpxh/remote_screen.height, lpxh/remote_screen.height);
        }

        // remote height is smaller
        if( lpxh - rpxh >= lpxh/tile_height ){
            resize_tile(lpxh/remote_screen.height, lpxh/remote_screen.height);
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
                "dl:   %5ldk %5ldKbit/s",
                tilecache.size(),
                ping_time[0], ping_time[1], ping_time[2],
                draw_time[0], draw_time[1], draw_time[2],
                scr_fetcher.total_dl / 1024,
                scr_fetcher.total_dl / ms * 8 * 1000 / 1024
                );
        SDL_Surface* text = FNT_Render(buf, (SDL_Color){0xff,0xff,0xff});
        SDL_Rect r = {0, 0, (Uint16)text->w, (Uint16)text->h};
        SDL_FillRect(g_screen.surface, &r, 0);
        SDL_BlitSurface(text, NULL, g_screen.surface, NULL);
        SDL_FreeSurface(text);
    }

    void draw(){
        SDL_Event event;
        vector<SDL_Event> events_queue;
        struct timeval t0,t1;

        scr_fetcher.fetch_screen(remote_screen);
        resize_tile(remote_screen.tile_width, remote_screen.tile_height);

        g_screen.resize( remote_screen.pixelWidth(), remote_screen.pixelHeight() );

        if(!g_screen.surface){
            error("SDL_SetVideoMode fail");
        }

        while(1){
          g_screen.resize_if_needed();

          gettimeofday(&t0, NULL);
          remote_screen.save();
          if( scr_fetcher.fetch_screen(remote_screen) && remote_screen.changed() ){
              gettimeofday(&t1, NULL);
              ping_time[0] = diff_ms(t1,t0);

              gettimeofday(&t0, NULL);
              check_sizes();

              for(int y=0; y<remote_screen.height; y++){
                  for(int x=0; x<remote_screen.width; x++){
                      uint32_t tile_id = remote_screen.at(x,y);
                      SDL_Surface*tile = tilecache[tile_id];

                      if( !tile ){
                          if(string *ps = scr_fetcher.fetch_tile(tile_id)){
                              SDL_RWops* rw    = SDL_RWFromConstMem(ps->data(), ps->size());
                              SDL_Surface* tmp = SDL_LoadBMP_RW(rw, 1); // auto frees rw
                              tile             = SDL_DisplayFormat(tmp);
                              SDL_FreeSurface(tmp);

                              tilecache[tile_id] = tile;
                              //printf("[d] tilecache: %ld tiles\n", tilecache.size());
                          }
                      }

                      if( !tile ){
                          // fetch failed, use default tile
                          tile = default_tile;
                          tilecache[tile_id] = tile;
                      }
                      
                      tileRect.x = x*tile_width;
                      tileRect.y = y*tile_height;
                      SDL_BlitSurface(tile, NULL, g_screen.surface, &tileRect);
                  }
              }

              gettimeofday(&t1, NULL);
              draw_time[0] = diff_ms(t1,t0);

              draw_info();

              /* update the g_screen.surface */
              //SDL_UpdateRect(g_screen.surface, 0, 0, 0, 0);
              SDL_Flip(g_screen.surface);
          } else {
              draw_info();
              SDL_Flip(g_screen.surface);
          }

          SDL_Delay(10);
        }

        /* clean up */
        //SDL_FreeSurface(sprite);
        SDL_Quit();
    }
};
