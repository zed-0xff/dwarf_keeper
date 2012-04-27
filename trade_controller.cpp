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
        string html;

        if(!trade_screen){
            return 
                "Please open Trade screen first."
                "<div class=comment>(highlight your Trade Depot and press 't' key)</div>";
        }

        if( request->get_int("m",0) == 1 ){
            html += "<script src='/jQuery.Tree.js'></script>\n";
            html += "<link rel=stylesheet type='text/css' href='/css/jQuery.Tree.css' />\n";
            html += "<script> $(function(){ $('#ltree').Tree() }) </script>\n";
            html += "<script> $(function(){ $('#rtree').Tree() }) </script>\n";
        } else {
            html += "<script src='/jstree.min.js'></script>\n";
            html += "<script> $(function(){ $('#ltree').jstree({plugins: ['themes','html_data','checkbox']}) }) </script>\n";
            html += "<script> $(function(){ $('#rtree').jstree({plugins: ['themes','html_data','checkbox']}) }) </script>\n";
        }

        html += "<div id=ltree>";
        html += items_tree(trade_screen->getLeftTradeVector());
        html += "</div>\n";

        html += "<div id=rtree>";
        html += items_tree(trade_screen->getRightTradeVector());
        html += "</div>\n";

        return html;
    }

    private:

    string items_tree(ItemsVector*v){
        string html;
        char buf[0x200];

        html += "<ul>\n";
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
            int price = (*it)->getValue();
            RefsVector* rv = (*it)->getRefs();
            for(int i=0; i<rv->size(); i++){
                if(rv->at(i)->getType() == Reference::REF_CONTAINS_ITEM){
                    price += rv->at(i)->getItem()->getValue();
                }
            }
            html += HTML::Item(link_to_item(*it), price);
        }
        html += "</table>\n";

        return html;
    }
};
