#include "common.h"

#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
    public:

    const char* link_to_item(Item* item, const char*title=NULL){
        static char buf[0x200];
        
        if(!item) return "";

        sprintf(buf, "<a href='/items?id=%d'>%s</a>", item->getId(), 
                html_escape(title ? title : item->getName()).c_str()
                );

        return buf;
    }
};

#endif
