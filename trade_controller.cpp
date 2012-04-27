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
                "<div class=comment>(highlight your Trade Depot and press 't' key)</div>"
                ;
        }

        html += "<table class='items sortable'>\n";
        html += "<tr><th>item <th class=sorttable_numeric>price\n";
        ItemsVector *v = trade_screen->getLeftTradeVector();
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

    private:
};
