#include <SDL/SDL.h>
#include <map>
#include "fetcher.cpp"
#include "async_fetcher.cpp"
#include <SDL_picofont.h>

using namespace std;

class Drawer {

    map <uint32_t, SDL_Surface*> tilecache;
    Screen remote_screen;

    int tile_width, tile_height;
    SDL_Rect tileRect;
    SDL_Surface *local_screen, *default_tile;
    Uint32 local_flags;

    int ping_time[4], draw_time[4];
    int steps;
    struct timeval t_start;

    public:
    Fetcher scr_fetcher, key_fetcher;

    Drawer(){
        default_tile = local_screen = NULL;
        local_flags = SDL_RESIZABLE|SDL_DOUBLEBUF;
        memset(ping_time, 0, sizeof(ping_time));
        memset(draw_time, 0, sizeof(draw_time));
        steps = 0;
        gettimeofday(&t_start, NULL);
    }

    void resize_window(int w, int h){
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

        if( local_screen ) SDL_FillRect(local_screen, NULL, 0);
    }

    void check_sizes(){
        int rpxw = remote_screen.width*tile_width;
        int lpxw = local_screen->w;
        int rpxh = remote_screen.height*tile_height;
        int lpxh = local_screen->h;

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
                "dl:   %5lldk %5lldKbit/s",
                tilecache.size(),
                ping_time[0], ping_time[1], ping_time[2],
                draw_time[0], draw_time[1], draw_time[2],
                scr_fetcher.total_dl / 1024,
                scr_fetcher.total_dl / ms * 8 * 1000 / 1024
                );
        SDL_Surface* text = FNT_Render(buf, (SDL_Color){0xff,0xff,0xff});
        SDL_Rect r = {0,0,text->w,text->h};
        SDL_FillRect(local_screen, &r, 0);
        SDL_BlitSurface(text, NULL, local_screen, NULL);
        SDL_FreeSurface(text);
    }

    void draw(){
        SDL_Event event;
        vector<SDL_Event> events_queue;
        struct timeval t0,t1;

        int colorkey, gameover;

        /* initialize SDL */
        SDL_Init(SDL_INIT_VIDEO);

        /* set the title bar */
        SDL_WM_SetCaption("SDL", "SDL");

        scr_fetcher.fetch_screen(remote_screen);
        resize_tile(remote_screen.tile_width, remote_screen.tile_height);

        /* create window */
        local_screen = SDL_SetVideoMode(
                remote_screen.pixelWidth(), 
                remote_screen.pixelHeight(), 
                0, local_flags);

        if(!local_screen){
            error("SDL_SetVideoMode fail");
        }

        /* setup sprite colorkey and turn on RLE */
//        colorkey = SDL_MapRGB(local_screen->format, 255, 0, 255);
//        SDL_SetColorKey(sprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

        gameover = 0;

        SDL_EnableUNICODE(1);

        /* message pump */
        while (!gameover)
        {
          events_queue.clear();
          while (SDL_PollEvent(&event)) {


//            printf("[.] %8x %8x %8x %8x %8x %8x\n", 
//                    ((int*)&event)[0], ((int*)&event)[1], ((int*)&event)[2],
//                    ((int*)&event)[3], ((int*)&event)[4], ((int*)&event)[5] 
//                    );

            /* an event was found */
            switch (event.type) {
              /* close button clicked */
              case SDL_QUIT:
                gameover = 1;
                events_queue.push_back(event);
                break;

              case SDL_VIDEORESIZE:
                printf("[d] resize %dx%d\n", event.resize.w, event.resize.h);
                local_screen = SDL_SetVideoMode( event.resize.w, event.resize.h, 0, local_flags);
                events_queue.push_back(event);
                break;

//              case SDL_MOUSEBUTTONDOWN:
//              case SDL_MOUSEBUTTONUP:
//                // don't support mouse wheel events (game screen zoom) for now
//                break;

              default:
                events_queue.push_back(event);
                break;

              /* handle the keyboard */
//              case SDL_KEYDOWN:
//                switch (event.key.keysym.sym) {
//                  case SDLK_ESCAPE:
//                  case SDLK_q:
//                    gameover = 1;
//                    break;
//                }
//                break;
            }

          }

          if( !events_queue.empty() ){
            key_fetcher.post_events(events_queue);
          }

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
                      SDL_BlitSurface(tile, NULL, local_screen, &tileRect);
                  }
              }

              gettimeofday(&t1, NULL);
              draw_time[0] = diff_ms(t1,t0);

              draw_info();

              /* update the local_screen */
              //SDL_UpdateRect(local_screen, 0, 0, 0, 0);
              SDL_Flip(local_screen);
          } else {
              draw_info();
              SDL_Flip(local_screen);
          }

          SDL_Delay(10);
        }

        /* clean up */
        //SDL_FreeSurface(sprite);
        SDL_Quit();
    }
};
