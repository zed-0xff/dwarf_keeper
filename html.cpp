#include "common.h"
#include "unicode.cpp"

class HTML{
    public:

    static const char* colored_item_name(::Item*item){
        static char buf[0x200];
        const char* add_class = "";
        const char* name = item->getName().c_str();

        bool is_worn   = (name[0] == 'X' && name[strlen(name)-1] == 'X');
        bool is_forbid = strchr(name,'{') && strchr(name,'}');

        if(is_forbid)
            sprintf(buf, "<span class=forbid>%s</span>", name);
        else if(is_worn)
            sprintf(buf, "<span class=worn>%s</span>", name);
        else
            return name;

        return buf;
    }

    static const char* item_color_classes(::Item*item){
        const char* name = item->getName().c_str();

        bool is_worn   = (name[0] == 'X' && name[strlen(name)-1] == 'X');
        bool is_forbid = strchr(name,'{') && strchr(name,'}');

        if(is_forbid)
            return " forbid";
        else if(is_worn)
            return " worn";
        else
            return "";
    }

    static const char* link_to_item(::Item* item, const char*title=NULL, const char*add=""){
        static char buf[0x200];
        
        if(!item) return "";

        sprintf(buf, "<a href='/items?id=%d'%s>%s</a>", 
                item->getId(),
                add,
                html_escape(title ? title : item->getName()).c_str()
                );

        return buf;
    }

    static char* Item(const char*name, int value, int flags=0, const char*add_class = ""){
        static char buf[0x200];

        bool is_worn   = (name[0] == 'X' && name[strlen(name)-1] == 'X');
        bool is_forbid = strchr(name,'{') && strchr(name,'}');

        if(is_forbid)
            add_class = " forbid";
        else if(is_worn)
            add_class = " worn";

        sprintf(buf, 
                "<tr>"
                    "<td class='name%s'>%s"
                    "<td class=r>%d<span class=currency>&#9788;</span>"
                    "\n",
          add_class,
          name,
          value
        );
        return buf;
    }

    static char* Item(::Item* item){
        return Item(link_to_item(item), item->getValue(), item->getFlags(), item_color_classes(item));
    }

    static string hexdump(void*ptr, size_t size, int width=0, string title = "", int row_width=0x10){
        string html;
        char buf[0x200];

        if(!title.empty()){
            html += "<title>" + html_escape(title) + "</title>\n";
            html += "<h2>" + html_escape(title) + "</h2>\n";
        }

        html += "<pre>";
        switch(width){
            case 4:{
                    uint32_t *p = (uint32_t*)ptr;
                    for(long i=0; i<size; i+=4, p++){
                        if(i % row_width == 0){ sprintf(buf, "\n%08lx: ", i); html += buf; }
                        sprintf(buf, " %08x", *p); 
                        html += buf;
                    }
                   }
                break;
            default:{
                    unsigned char*p = (unsigned char*)ptr;
                    for(long i=0; i<size; i++, p++){
                        if(i % row_width == 0){ sprintf(buf, "\n%08lx: ", i); html += buf; }
                        sprintf(buf, " %02x", *p); 
                        html += buf;
                    }
                    }
                break;
        }
        html += "</pre>";

    return html;
    }
};
