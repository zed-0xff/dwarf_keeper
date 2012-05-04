#include "controller.cpp"
#include "screen.cpp"
#include "window.cpp"

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
        if( strstr(request->url, "/draw")){
            return draw();
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

    string draw(){
        string html;
        unsigned char c,bg,fg, bg0=0, fg0=0;
        char buf[0x200];
        bool color = true;

        html.reserve( color ? 30000 : 5000);

        Window* w = Window::root();

        html += "<pre class=pseudographics>";
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
        html += "</pre>";

        return html;
    }
};
