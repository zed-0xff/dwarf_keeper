#include "controller.cpp"

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

        string html;
        html.reserve(400*1024);
        html += "<div id=trade>\n";

        switch( request->get_int("m",0) ){
            case 1:
                // jQuery.Tree
                html += "<script src='/jQuery.Tree.js'></script>\n";
                html += "<link rel=stylesheet type='text/css' href='/css/jQuery.Tree.css' />\n";
                html += "<script> $(function(){ $('#ltree').Tree() }) </script>\n";
                html += "<script> $(function(){ $('#rtree').Tree() }) </script>\n";
                break;
            case 2:
                // jstree
                html += "<script src='/jstree.min.js'></script>\n";
                html += "<script> $(function(){ $('#ltree').jstree({plugins: ['themes','html_data','checkbox']}) }) </script>\n";
                html += "<script> $(function(){ $('#rtree').jstree({plugins: ['themes','html_data','checkbox']}) }) </script>\n";
                break;
        }

        html += "<div id=ltree>";
        html += items_treetable(trade_screen->getLeftTradeVector());
        html += "</div>\n";

        html += "<div id=rtree>";
        html += items_treetable(trade_screen->getRightTradeVector());
        html += "</div>\n";

        html += "</div>\n";

        return html;
    }

    private:

    string items_treetable(ItemsVector*v){
        string html, chunk;
        char buf[0x200];
        int sum_price;

        html += "<table class='sortable items'>\n";
        html += "<thead>"
                    "<tr>"
                        "<th class='sorttable_nosort'>"
                        "<th class='sorttable_alpha th-item-name' >item"
                        "<th class='sorttable_numeric'>price"
                "</thead>\n";

        // TODO: optimize loop
        for( int type_id=0; type_id<100; type_id++){
            chunk.clear();
            sum_price = 0;

            for( ItemsVector::iterator it = v->begin(); it < v->end(); it++){
                Item *item = *it;
                if(item->getTypeId() != type_id) continue;

                int price = item->getPrice();
                sum_price += price;

                sprintf(buf,
                        "<tr>"
                            "<td>"
                            "<td>"
                                "<input name=i%d type=checkbox>"
                                "%s"
                            "<td class=r>"
                                "%d<span class=currency>&#9788;</span>"
                        "\n",
                        item->getId(),
                        HTML::colored_item_name(item),
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

    string items_tree(ItemsVector*v){
        string html;
        char buf[0x200];

        html += "<ul>\n";
        // TODO: optimize loop
        for( int type_id=0; type_id<100; type_id++){
            bool was = false;
            for( ItemsVector::iterator it = v->begin(); it < v->end(); it++){
                if((*it)->getTypeId() != type_id) continue;
                if(!was){
                    was = true;
                    sprintf(buf, "<li><label>type %d</label>\n\t<ul>\n", type_id); html += buf;
                }
                html += "\t<li><label>";
                html += link_to_item(*it);
                html += "</label>\n";
            }
            if(was) html += "\t</ul>\n";
        }
        html += "</ul>";
        return html;
    }

    string items_table(ItemsVector*v){
        string html;

        html += "<table class='items sortable'>\n";
        html += "<tr><th>item <th class=sorttable_numeric>price\n";
        //ItemsVector *v = trade_screen->getLeftTradeVector();
        for( ItemsVector::iterator it = v->begin(); it < v->end(); it++){
            html += HTML::Item(link_to_item(*it), (*it)->getPrice());
        }
        html += "</table>\n";

        return html;
    }
};
