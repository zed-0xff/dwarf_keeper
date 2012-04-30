#include "controller.cpp"
#include "screen.cpp"

class TradeController : Controller {
    HTTPRequest* request;
    Screen* trade_screen;

    public:
    int resp_code;

    TradeController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
        trade_screen = NULL;

        for( Screen *scr = Screen::root(); scr; scr = scr->next() ){
            const char* name = scr->vtbl2type();
            if(name && strstr(name,"viewscreen_tradegoodsst")){
                trade_screen = scr;
                break;
            }
        }
    }

    string to_html(){
        if(!trade_screen){
            return "Please open Trade screen first."
                   "<div class=comment>(highlight your Trade Depot and press 't' key)</div>";
        }

        vector<int> item_ids = request->get_ints("id"); // item id
        int type_id = request->get_int("type_id", -1);
        int state   = request->get_int("state",   -1);

        if( state != -1 ){
            if( item_ids.size() > 0 ){
                if( string *ps = toggle_items(item_ids, state) ){
                    return *ps;
                } else {
                    resp_code = MHD_HTTP_NOT_FOUND;
                    return "Not Found";
                }
            }

            if( type_id != -1 ){
                string side = request->get_string("side","");
                int side_id;

                if(side == "left") 
                    side_id = Screen::TRADE_SIDE_LEFT;
                else if(side == "right")
                    side_id = Screen::TRADE_SIDE_RIGHT;
                else {
                    resp_code = MHD_HTTP_BAD_REQUEST;
                    return "invalid side";
                }

                if( string *ps = toggle_type(type_id, side_id, state) ){
                    return *ps;
                } else {
                    resp_code = MHD_HTTP_NOT_FOUND;
                    return "Not Found";
                }
            }

            resp_code = MHD_HTTP_BAD_REQUEST;
            return "invalid args";
        }

        string html;
        html.reserve(400*1024);
        html += "<div id=trade>\n";

        html += "<div id=ltree>";
        html += items_treetable(trade_screen->getTradeSideInfo(Screen::TRADE_SIDE_LEFT), "side_left");
        html += "</div>\n";

        html += "<div id=rtree>";
        html += items_treetable(trade_screen->getTradeSideInfo(Screen::TRADE_SIDE_RIGHT), "side_right");
        html += "</div>\n";

        html += "</div>\n";

        return html;
    }

    private:

    string* toggle_type(int type_id, int side, int state){
        static string js;
        char buf[0x40];
        set<int> types;

        js = "{\"ids\":[";
        TradeSideInfo tsi = trade_screen->getTradeSideInfo(side);
        for( int i=0; i < tsi.items->size(); i++){
            Item *item = tsi.items->at(i);
            if(item->getTypeId() != type_id) continue;

            tsi.toggle_by_index(i, state);
            if(state){
                // item is CHECKED
                // now a) uncheck item container, if any
                //     b) uncheck all item containees, if any
                RefsVector *rv = item->getRefs();
                for(int j=0; j<rv->size(); j++){
                    Reference *ref = rv->at(j);
                    switch(ref->getType()){
                        case Reference::REF_CONTAINED_IN_ITEM:
                        case Reference::REF_CONTAINS_ITEM:
                            Item* ref_item = ref->getItem();
                            int ref_id = ref_item->getId();

                            tsi.toggle_item(ref_id, false);

                            types.insert(ref_item->getTypeId());
                            sprintf(buf, "%d,", ref_id); js += buf;
                            break;
                    }
                }
            }
        }
        if(js.size() > 1 && js[js.size()-1] == ',') js.erase(js.size()-1);
        js += "], \"types\":[";
        for(set<int>::iterator it = types.begin(); it != types.end(); it++){
            sprintf(buf, "%d,", *it); js += buf;
        }
        if(js.size() > 1 && js[js.size()-1] == ',') js.erase(js.size()-1);
        js += "]}";
        return &js;
    }

    string* toggle_items(vector<int>ids, int state){
        static string js;
        char buf[0x40];
        set<int> types;

        js = "{\"ids\":[";
        for( int i = 0; i<ids.size(); i++ ){
            int id = ids[i];
            for( int side = Screen::TRADE_SIDE_LEFT; side <= Screen::TRADE_SIDE_RIGHT; side++ ){
                TradeSideInfo tsi = trade_screen->getTradeSideInfo(side);
                if( Item* item = tsi.toggle_item(id, state) ){
                    if(state){
                        // item is CHECKED
                        // now a) uncheck item container, if any
                        //     b) uncheck all item containees, if any
                        RefsVector *rv = item->getRefs();
                        for(int j=0; j<rv->size(); j++){
                            Reference *ref = rv->at(j);
                            switch(ref->getType()){
                                case Reference::REF_CONTAINED_IN_ITEM:
                                case Reference::REF_CONTAINS_ITEM:
                                    Item* ref_item = ref->getItem();
                                    int ref_id = ref_item->getId();

                                    tsi.toggle_item(ref_id, false);

                                    types.insert(ref_item->getTypeId());
                                    sprintf(buf, "%d,", ref_id); js += buf;
                                    break;
                            }
                        }
                    }
                }
            }
        }
        if(js.size() > 1 && js[js.size()-1] == ',') js.erase(js.size()-1);
        js += "], \"types\":[";
        for(set<int>::iterator it = types.begin(); it != types.end(); it++){
            sprintf(buf, "%d,", *it); js += buf;
        }
        if(js.size() > 1 && js[js.size()-1] == ',') js.erase(js.size()-1);
        js += "]}";
        return &js;
    }

    string* toggle_item(int id, int state){
        static string js;
        char buf[0x40];
        set<int> types;

        js = "{\"ids\":[";
        for( int side = Screen::TRADE_SIDE_LEFT; side <= Screen::TRADE_SIDE_RIGHT; side++ ){
            TradeSideInfo tsi = trade_screen->getTradeSideInfo(side);
            if( Item* item = tsi.toggle_item(id, state) ){
                if(state){
                    // item is CHECKED
                    // now a) uncheck item container, if any
                    //     b) uncheck all item containees, if any
                    RefsVector *rv = item->getRefs();
                    for(int j=0; j<rv->size(); j++){
                        Reference *ref = rv->at(j);
                        switch(ref->getType()){
                            case Reference::REF_CONTAINED_IN_ITEM:
                            case Reference::REF_CONTAINS_ITEM:
                                Item* ref_item = ref->getItem();
                                int ref_id = ref_item->getId();

                                tsi.toggle_item(ref_id, false);

                                types.insert(ref_item->getTypeId());
                                sprintf(buf, "%d,", ref_id); js += buf;
                                break;
                        }
                    }
                }
                if(js.size() > 1 && js[js.size()-1] == ',') js.erase(js.size()-1);
                js += "], \"types\":[";
                for(set<int>::iterator it = types.begin(); it != types.end(); it++){
                    sprintf(buf, "%d,", *it); js += buf;
                }
                if(js.size() > 1 && js[js.size()-1] == ',') js.erase(js.size()-1);
                js += "]}";
                return &js;
            }
        }
        return NULL;
    }

    string items_treetable( TradeSideInfo tsi, const char*side ){
        string html, chunk;
        char buf[0x200];
        int sum_price;

        sprintf(buf,
                "<table class='sortable items %s'>\n"
                    "<thead>"
                        "<tr>"
                            "<th class='sorttable_nosort'>"
                            "<th class='sorttable_alpha th-item-name' >item"
                            "<th class='sorttable_numeric'>price"
                    "</thead>\n",
                side); html += buf;

        // TODO: optimize loop
        for( int type_id=0; type_id<100; type_id++){
            chunk.clear();
            sum_price = 0;

            for( int i=0; i < tsi.items->size(); i++){
                Item *item = tsi.items->at(i);
                if(item->getTypeId() != type_id) continue;

                int price = item->getPrice();
                sum_price += price;

                sprintf(buf,
                        "<tr>"
                            "<td class=info>"
                                "%s"
                            "<td class='name%s'>"
                                "<input id=i%d type=checkbox%s>"
                                "%s"
                            "<td class=r>"
                                "%d<span class=currency>&#9788;</span>"
                        "\n",
                        link_to_item(item,""," class=info target=_blank"),
                        HTML::item_color_classes(item),
                        item->getId(),
                        tsi.checks->at(i) ? " CHECKED" : "",
                        item->getName().c_str(),
                        price
                       );
                chunk += buf;
            }

            if(!chunk.empty()){
                sprintf(buf, 
                        "<tbody class=category id=cat%d>"
                            "<tr>"
                                "<th class=tristate>"
                                "<th class=category-name>%s"
                                "<th class=r>%d<span class=currency>&#9788;</span>\n"
                        "<tbody class=cat_items id=tb%d>\n", 
                        type_id, ItemType::type2string(type_id), sum_price, type_id); html += buf;
                html += chunk;
            }
        }
        html += "</table>";
        return html;
    }

};
