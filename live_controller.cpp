#include "controller.cpp"
#include "window.cpp"
#include "offscreen_renderer.cpp"
#include "copied_screen.cpp"

#include <queue>

class LiveController : Controller {
    HTTPRequest* request;
    static CopiedScreen copied_screen;
    static void* SDL_PushEvent_func;

    public:
    int resp_code;
    MHD_Response* response;

    LiveController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
        response = NULL;
    }

    string to_html(){
        if( strstr(request->url, "/dump")){
            return dump();
        }
        if( strstr(request->url, "/ws")){
            return websocket();
        }
        if( strstr(request->url, "live.bmp")){
            return bmp();
        }
        if( strstr(request->url, "live.bin")){
            return bin();
        }
        if( strstr(request->url, "tiles.bin")){
            return tiles();
        }
        if( request->url_match("/live/tile.bmp")){
            return tile(false);
        }
        if( request->url_match("/live/tile.snp")){
            return tile(true);
        }
        if( strstr(request->url, "sprite.bmp")){
            return sprite();
        }
        if( request->url_match("/live") ){
            return live();
        }
        if( request->url_match("/live/sdl_events") ){
            return sdl_events();
        }
        if( request->url_match("/live/detach") ){
            return detach();
        }

        resp_code = MHD_HTTP_NOT_FOUND;
        return "Unknown url";
    }

    static void copy_screen(){
        copied_screen.copy();
    }

    private:

    void push_event(SDL_Event*event){
        if(!event) return;
        if(!SDL_PushEvent_func){
            SDL_PushEvent_func = dlsym(RTLD_DEFAULT, "SDL_PushEvent");
            if(!SDL_PushEvent_func){
                printf("[!] dlsym SDL_PushEvent returned NULL! Cannot inject events!");
                return;
            }
        }
        ((func_t_p)SDL_PushEvent_func)(event);
    }

    string detach(){
        int fd = request->getConnection()->socket_fd;
        printf("[d] fd=%d\n", fd);
        for(int i=0;i<100;i++){
            send(fd, "foo", 3, 0);
        }
        request->getConnection()->socket_fd = -1;
        return "OK";
    }

    string sdl_events(){
        char buf[0x200];
        int n = request->get_int("n", 0);

        for(int i=0; i<n; i++){
            sprintf(buf, "e%d", i);

            vector<uint32_t> v = request->get_uints(buf);
            if( v.size() == 0) break; // NULL event?

            SDL_Event ev;
            bzero(&ev, sizeof(ev));
            uint32_t*p = (uint32_t*)&ev;

            for(int i=0; i<min(sizeof(ev)/4, v.size()); i++, p++){
                *p = v[i];
            }

            push_event(&ev);
        }

        return "OK";
    }

    string sprite(){
        // TODO: check width/height
        uint32_t*p = (uint32_t*)gps.screen;

        if(!p){
            return "ERROR: gps.screen is NULL";
        }

        if(gps.screentexpos) memset(gps.screentexpos, 0, gps.dimx*gps.dimy*sizeof(long));

        for(int i=0; i<=0x2ff; i++) p[i] = i;

        return bmp();
    }

    const char* tile(bool compress=false){
        uint32_t id = request->get_uint("id", 0);
        if( !id ){
            resp_code = MHD_HTTP_NOT_FOUND;
            return "ERROR: no tile id";
        }

        bool found;
        tile_fullid tile = copied_screen.tileno2tile(id, &found);

        if( !found ){
            resp_code = MHD_HTTP_NOT_FOUND;
            return "ERROR: tile not found";
        }

        int size = 10*1024;
        void *p = malloc(size);

        if( gps.screen) *(uint32_t*)gps.screen = tile.c;
        if( gps.screentexpos ){
            if( tile.texpos ){
                *gps.screentexpos = tile.texpos;
                if( gps.screentexpos_addcolor ) *gps.screentexpos_addcolor = tile.ad;
                if( gps.screentexpos_grayscale) *gps.screentexpos_grayscale= tile.gr;
                if( gps.screentexpos_cf       ) *gps.screentexpos_cf       = tile.cf;
                if( gps.screentexpos_cbr      ) *gps.screentexpos_cbr      = tile.cb;
            } else {
                *gps.screentexpos = 0;
            }
        }
        OffscreenRenderer r(1, 1);
        r.render(0,0);
        r.save(p, size);

        int bmsize = *(int*)((char*)p+2); // BITMAPFILEHEADER.bfSize
        if( bmsize < 1 || bmsize > size){
            return "ERROR: bmp size";
        }

        if( compress ){
            string compressed;
            snappy::Compress((const char*)p, bmsize, &compressed);
            //printf("[d] compress %d -> %d\n", bmsize, compressed.size());
            response = MHD_create_response_from_data(
                    compressed.size(), 
                    (void*)compressed.data(), 
                    0, 1); // don't free, copy
        } else {
            // free, don't copy
            response = MHD_create_response_from_data(bmsize, p, 1, 0);
        }

        return "OK";
    }

    const char* tiles(){
        vector<uint32_t> ids = request->get_uints("id");
        if( ids.size() <= 0 ){
            resp_code = MHD_HTTP_NOT_FOUND;
            return "ERROR: no tile ids";
        }

        int size = ids.size()*4000; // TODO: size
        void *p = malloc(size);

        if(gps.screentexpos) gps.screentexpos[0] = 0;

        *(uint32_t*)gps.screen = ids[0];
        OffscreenRenderer r(1, 1);
        r.render(0,0);
        r.save(p, size);

        response = MHD_create_response_from_data(size, p, 1, 0);
        return "OK";
    }

    string bin(){
        const char* r = "OK";

        copied_screen.lock();
        if( copied_screen.valid() ){
            size_t size;
            string *pdata;
            uint32_t hash = request->get_uint("h", 0);

            if( copied_screen.changed(hash) ){
                // screen changed or no hash
                pdata = copied_screen.prepare_data();
                response = MHD_create_response_from_data( pdata->size(), (void*)pdata->data(), 0, 1); // don't free, copy
            } else {
                // screen NOT changed
                // should wait for semaphore/spinlock/whatever
                copied_screen.wait();
                if( copied_screen.changed(hash) ){
                    // changed after wait
                    pdata = copied_screen.prepare_data();
                    response = MHD_create_response_from_data( pdata->size(), (void*)pdata->data(), 0, 1); // don't free, copy
                }
            }
        } else {
            resp_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
            r = "Error: no screen";
        }
        copied_screen.unlock();
        return r;
    }

    string bmp(){
        int w = request->get_int("w", gps.dimx);
        int h = request->get_int("h", gps.dimy);
        OffscreenRenderer r(w, h);
        r.render(request->get_int("x",0), request->get_int("y",0));

        int bufsize = w*h*1000 + 90; // rough bmp size
        char*buf = (char*)malloc(bufsize);
        if( !buf ) return "ERROR in malloc()";
        if( r.save(buf, bufsize) ){
            int size = *(int*)(buf+2); // BITMAPFILEHEADER.bfSize
            if( size < 1 || size > bufsize){
                return "ERROR: bmp size";
            }
            response = MHD_create_response_from_data(size, buf, 1, 0);
            return "OK";
        } else {
            return "ERROR in r.save()";
        }
    }

    // TODO: sha1 + base64: http://ru.wikipedia.org/wiki/WebSocket#.D0.9F.D1.80.D0.BE.D1.82.D0.BE.D0.BA.D0.BE.D0.BB_07
    string websocket(){
        resp_code = MHD_HTTP_SWITCHING_PROTOCOLS;
        // not implemented yet in libmicrohttpd
        //response = MHD_create_response_for_upgrade(upgrade_handler, NULL);
        response = MHD_create_response_from_data(0,(void*)"",0,0);
        //int fd = request->connection.socket_fd;
        MHD_add_response_header(response, "Upgrade",    "websocket");
        MHD_add_response_header(response, "Connection", "Upgrade");
        return "OK";
    }

    string dump(){
        char title[0x200];

        size_t char_size = 4;
        size_t size = gps.dimx * gps.dimy;

        sprintf(title, "videobuf (%dx%d)", gps.dimx, gps.dimy);

        if( size > 40000 ){
            strcat(title, " [size too big, limited to 40000]");
            size = 40000;
        }

        return HTML::hexdump(gps.screen, size*char_size, char_size, title, gps.dimy * char_size);
    }

    string live(){
        string html;
        unsigned char c,bg,fg, bg0=0, fg0=0;
        char buf[0x200];
        bool color = true;
        bool is_ajax = request->is_ajax();

        int key = request->get_int("key",0);
        if(key){
            int mod = 0;
            uint16_t ukey = request->get_int("ukey", key);

            SDL_Event ev;
            bzero(&ev, sizeof(ev));

            if(request->get_int("alt",0))   mod |= KMOD_LALT;
            if(request->get_int("ctrl",0))  mod |= KMOD_LCTRL;
            if(request->get_int("shift",0)) mod |= KMOD_LSHIFT;
            if(request->get_int("meta",0))  mod |= KMOD_LMETA;

            ev.type                = SDL_KEYDOWN;
            ev.key.state           = SDL_PRESSED;
            ev.key.keysym.scancode = 0;

            // hold left alt
            if( mod & KMOD_LALT ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LALT;
                ev.key.keysym.unicode  = 0;
                push_event(&ev);
            }

            // hold left ctrl
            if( mod & KMOD_LCTRL ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LCTRL;
                ev.key.keysym.unicode  = 0;
                push_event(&ev);
            }

            ev.key.keysym.mod      = (SDLMod)mod;
            ev.key.keysym.sym      = (SDLKey)key;
            ev.key.keysym.unicode  = ukey;

            push_event(&ev);

            ev.type                = SDL_KEYUP;
            ev.key.state           = SDL_RELEASED;
            // Key release events (SDL_KEYUP) won't necessarily (ever?) contain unicode information. 
            // http://lists.libsdl.org/pipermail/sdl-libsdl.org/2005-January/048355.html)
            ev.key.keysym.unicode  = 0;
            push_event(&ev);

            // release left ctrl
            if( mod & KMOD_LCTRL ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LCTRL;
                ev.key.keysym.unicode  = 0;
                push_event(&ev);
            }

            // release left alt
            if( mod & KMOD_LALT ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LALT;
                ev.key.keysym.unicode  = 0;
                push_event(&ev);
            }

            return "QUEUED";
        }

        html.reserve( color ? 30000 : 5000);

        if(is_ajax){
            uint32_t hash = 0;
            for( int y = 0; y<gps.dimy; y++){
                for( int x = 0; x<gps.dimx; x++){
                    hash ^= ((uint32_t*)gps.screen)[x*gps.dimy+y];
                    hash = hash << 1 | hash >> 31;     // circular shift 1 bit left
                }
            }
            sprintf(buf, "%x", hash);
            if( request->get_string("hash","") == buf){
                return "NOT_MODIFIED";
            }
            sprintf(buf, "<span id='live-hash' style='display:none'>%x</span>", hash);
            html += buf;
        } else {
            html += "<pre id='live' class=pseudographics>";
        }

        for( int y = 0; y<gps.dimy; y++){
            for( int x = 0; x<gps.dimx; x++){
                int idx = (x*gps.dimy+y)*4;
                c  = gps.screen[idx];
                fg = gps.screen[idx+1];
                bg = gps.screen[idx+2];

                if(bg0 != bg || fg0 != fg){
                    if(bg0 + fg0 > 0) html += "</span>";
                    if(bg+fg > 0){
                        sprintf(buf, "<span class='bg%x fg%x'>", bg, fg);
                        html += buf;
                    }
                    bg0 = bg; fg0 = fg;
                }

                html += CP437_TO_UTF8[c];
            }
            html += "\n";
        }
        if(bg0 + fg0 > 0) html += "</span>";

        if(!is_ajax) html += "</pre>";

        return html;
    }
};

CopiedScreen LiveController::copied_screen;
void* LiveController::SDL_PushEvent_func = NULL;
