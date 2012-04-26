#include "common.h"

class HTML{
    public:

    static char* Item(const char*name, int value, int flags=0){
        static char buf[0x200];
        const char* add_class = "";

        bool is_worn   = (name[0] == 'X' && name[strlen(name)-1] == 'X');
        bool is_forbid = strchr(name,'{') && strchr(name,'}');

        if(is_forbid)
            add_class = " forbid";
        else if(is_worn)
            add_class = " worn";

        sprintf(buf, 
                "<tr>"
                    "<td class='name%s'>%s"
                    "<td class='value r'>%d<span class=currency>&#9788;</span>"
                    "\n",
          add_class,
          name,
          value
        );
        return buf;
    }

    static char* Item(::Item* pItem){
        return Item(pItem->getName().c_str(), pItem->getValue(), pItem->getFlags());
    }
};
