#include "controller.cpp"
#include "screen.cpp"
#include "window.cpp"

#include <queue>

static queue<SDL_Event> g_override_keys;

class ScreenController : Controller {
    HTTPRequest* request;
    Coords coords;
    int unit_id;

    public:
    int resp_code;

    ScreenController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;

        coords.x = request->get_int("x",-1);
        coords.y = request->get_int("y",-1);
        coords.z = request->get_int("z",-1);

        unit_id  = request->get_int("unit_id",-1);
    }

    string to_html(){
        if( strstr(request->url, "/dump")){
            return dump();
        }
        if( strstr(request->url, "/live")){
            return live();
        }

        if( unit_id != -1 ){
            if(Unit *unit = Unit::find(unit_id)){
                // XXX HACK
                *((int*)0x1563A3C) = -30000;
                *((int*)0x1563A48) = -30000;
                *((uint16_t*)0x156A97C) = 0x17;
                *((int*)0x1563B5C) = -1;
                *((int*)0x1563B68) = 0;
                *((uint8_t*)0x16FEA54) = 1;
                *((uint8_t*)0x16FEA55) = 1;

                Screen::moveTo(unit->getCoords());
                ((func_t_i)(FOO_FUNC))(unit_id);        // open unit info right panel
            } else {
                resp_code = MHD_HTTP_NOT_FOUND;
                return "Unit not found";
            }
        } else if( coords.x != -1 && coords.y != -1 && coords.z != -1 ){
            Screen::moveTo(coords);
        }

        coords = Screen::getCenter();
        char buf[0x100];
        sprintf(buf, "<a class=coords href='?x=%d&y=%d&z=%d'>(%d,%d,%d)</a>\n",
                coords.x, coords.y, coords.z,
                coords.x, coords.y, coords.z
                );

        return buf;
    }

    private:

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
