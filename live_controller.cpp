#include "controller.cpp"
#include "window.cpp"
#include "offscreen_renderer.cpp"

#include <queue>

static char NULL_DATA[] = "";

static queue<SDL_Event> g_override_keys;

class LiveController : Controller {
    HTTPRequest* request;

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
        if( strstr(request->url, ".bmp")){
            return save();
        }

        return live();
    }

    private:

    string save(){
        int w = request->get_int("w", 20);
        int h = request->get_int("h", 20);
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
        response = MHD_create_response_from_data(0,NULL_DATA,0,0);
        //int fd = request->connection.socket_fd;
        MHD_add_response_header(response, "Upgrade",    "websocket");
        MHD_add_response_header(response, "Connection", "Upgrade");
        return "OK";
    }

    string dump(){
        char title[0x200];
        Window* w = Window::root();

        size_t char_size = sizeof(*w->vbuf);
        size_t size = (w->max_x+1) * (w->max_y+1);

        sprintf(title, "videobuf (%dx%d)", w->max_x+1, w->max_y+1);

        if( size > 10000 ){
            strcat(title, " [size too big, limited to 10000]");
            size = 10000;
        }

        return HTML::hexdump(w->vbuf, size*char_size, char_size, title, (w->max_y+1) * char_size);
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
                g_override_keys.push(ev);
            }

            // hold left ctrl
            if( mod & KMOD_LCTRL ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LCTRL;
                ev.key.keysym.unicode  = 0;
                g_override_keys.push(ev);
            }

            ev.key.keysym.mod      = (SDLMod)mod;
            ev.key.keysym.sym      = (SDLKey)key;
            ev.key.keysym.unicode  = ukey;

            g_override_keys.push(ev);

            ev.type                = SDL_KEYUP;
            ev.key.state           = SDL_RELEASED;
            // Key release events (SDL_KEYUP) won't necessarily (ever?) contain unicode information. 
            // http://lists.libsdl.org/pipermail/sdl-libsdl.org/2005-January/048355.html)
            ev.key.keysym.unicode  = 0;
            g_override_keys.push(ev);

            // release left ctrl
            if( mod & KMOD_LCTRL ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LCTRL;
                ev.key.keysym.unicode  = 0;
                g_override_keys.push(ev);
            }

            // release left alt
            if( mod & KMOD_LALT ){
                ev.key.keysym.mod      = (SDLMod)0;
                ev.key.keysym.sym      = SDLK_LALT;
                ev.key.keysym.unicode  = 0;
                g_override_keys.push(ev);
            }

            return "QUEUED";
        }

        html.reserve( color ? 30000 : 5000);

        Window* w = Window::root();

        if(is_ajax){
            uint32_t hash = 0;
            for( int y = 0; y<=w->max_y; y++){
                for( int x = 0; x<=w->max_x; x++){
                    hash ^= w->vbuf[x*(w->max_y+1)+y];
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

        for( int y = 0; y<=w->max_y; y++){
            for( int x = 0; x<=w->max_x; x++){
                c = w->vbuf[x*(w->max_y+1)+y] & 0xff;
                fg = ( w->vbuf[x*(w->max_y+1)+y] >> 8 ) & 0xff;
                bg = ( w->vbuf[x*(w->max_y+1)+y] >> 16 ) & 0xff;

                if(bg0 != bg || fg0 != fg){
                    if(bg0 + fg0 > 0) html += "</span>";
                    if(bg+fg > 0){
                        sprintf(buf, "<span class='bg%x fg%x'>", bg, fg);
                        html += buf;
                    }
                    bg0 = bg; fg0 = fg;
                }

                switch(c){
                    case 0x00: html += ' '; break;
                    case 0x09:
                        html += "&#9675;"; // "○" - well
                        break;
                    case 0x0a:
                        html += "&#9702;"; // "◦" - mined ores: gold nuggets, limonite, garnierite ...
                        break;
                    case 0x0d: html += "&#9834;"; break; // "♪"
                    default:
                        html += c;
                        break;
                }
            }
            html += "\n";
        }
        if(bg0 + fg0 > 0) html += "</span>";

        if(!is_ajax) html += "</pre>";

        return html;
    }
};
