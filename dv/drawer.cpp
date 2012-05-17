#include <SDL/SDL.h>
#include <map>
#include "fetcher.cpp"

using namespace std;

#define TILE_SIZE      18

class Drawer {

    map <uint32_t, SDL_Surface*> tilecache;
    Fetcher fetcher;
    Screen remote_screen;

    public:

    void draw(){
        SDL_Surface *local_screen, *default_tile;
        SDL_Rect tileRect;
        SDL_Event event;
        vector<SDL_Event> events_queue;

        int colorkey, gameover;

        /* initialize SDL */
        SDL_Init(SDL_INIT_VIDEO);

        /* set the title bar */
        SDL_WM_SetCaption("SDL Move", "SDL Move");

        fetcher.fetch_screen(remote_screen);

        /* create window */
        local_screen = SDL_SetVideoMode(
                remote_screen.pixelWidth(), 
                remote_screen.pixelHeight(), 
                0, SDL_RESIZABLE);

        if(!local_screen){
            SDL_Quit();
            exit(1);
        }

        /* setup sprite colorkey and turn on RLE */
//        colorkey = SDL_MapRGB(local_screen->format, 255, 0, 255);
//        SDL_SetColorKey(sprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

        gameover = 0;

        default_tile = SDL_CreateRGBSurface( 
                SDL_SWSURFACE, 
                remote_screen.tile_width,
                remote_screen.tile_height,
                32,0,0,0,0);

        if(sizeof(SDL_Event) != 24) throw("assumed that sizeof(SDL_Event) == 24, now it's not!");

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

              case SDL_MOUSEBUTTONDOWN:
              case SDL_MOUSEBUTTONUP:
                // don't support mouse wheel events (game screen zoom) for now
                break;

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
            fetcher.post_events(events_queue);
          }

          tileRect.w = remote_screen.tile_width;
          tileRect.h = remote_screen.tile_height;

          remote_screen.save();
          fetcher.fetch_screen(remote_screen);

          if( remote_screen.changed() ){
              for(int y=0; y<remote_screen.height; y++){
                  for(int x=0; x<remote_screen.width; x++){
                      uint32_t tile_id = remote_screen.at(x,y);
                      SDL_Surface*tile = tilecache[tile_id];

                      if( !tile ){
                          if(string *ps = fetcher.fetch_tile(tile_id)){
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
                      
                      tileRect.x = x*remote_screen.tile_width;
                      tileRect.y = y*remote_screen.tile_height;
                      SDL_BlitSurface(tile, NULL, local_screen, &tileRect);
                  }
              }

              /* update the local_screen */
              //SDL_UpdateRect(local_screen, 0, 0, 0, 0);
              SDL_Flip(local_screen);
          } else {
              //printf("[d] no screen changes\n");
          }

          SDL_Delay(100);
        }

        /* clean up */
        //SDL_FreeSurface(sprite);
        SDL_Quit();
    }
};
