#include "common.h"

#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
    public:

    const char* link_to_item(Item* item, const char*title=NULL, const char*add=""){
        static char buf[0x200];
        
        if(!item) return "";

        sprintf(buf, "<a href='/items?id=%d'%s>%s</a>", 
                item->getId(),
                add,
                html_escape(title ? title : item->getName()).c_str()
                );

        return buf;
    }

    const char* link_to_unit(Unit *unit){
        static char buf[0x400];
        
        if(!unit) return "";

        sprintf(buf, "<a href='/units?id=%d'>%s</a>", 
                unit->getId(),
                html_escape(unit->getName()).c_str()
                );

        return buf;
    }

    const char* link_to_building(Building*b){
        static char buf[0x400];
        
        if(!b) return "";

        sprintf(buf, "<a href='/buildings?id=%d'>%s</a>", 
                b->getId(),
                html_escape(b->getName()).c_str()
                );

        return buf;
    }

    const char* link_to_coords(Coords c){
        static char buf[0x400];
        
        sprintf(buf, "<a class=coords href='/screen?x=%d&y=%d&z=%d'>(%d,%d,%d)</a>", 
                c.x, c.y, c.z,
                c.x, c.y, c.z
                );

        return buf;
    }
};

#endif
