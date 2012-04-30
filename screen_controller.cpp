#include "controller.cpp"
#include "screen.cpp"

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
};
