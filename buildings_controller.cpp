#include "controller.cpp"

class BuildingsController : Controller {
    HTTPRequest* request;

    public:
    int resp_code;

    BuildingsController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
    }

    string to_html(){
        int id;
        string html;

        if( -1 != (id = request->get_int("id", -1))){
            if(Building* b = Building::find(id)){
                return show(b);
            } else {
                resp_code = MHD_HTTP_NOT_FOUND;
                return "Not Found";
            }
        }

        html += "<div id=buildings>\n";
        html += index();
        html += "</div>\n";

        return html;
    }

    private:

    string show(Building *b){
        string html;
        char buf[0x1000];
        string name = b->getName();

        html += "<div id=building>\n";
        html += "<h1>" + name + "</h1>\n";

        // general building info

        html += "<table class=t1>\n";

        html += "<tr><th> name <td>" + name + "\n";

        html += "<tr><th>coords <td class=r>";
        html += link_to_coords(b->getCoords());

        sprintf(buf, "<tr><th>flags <td class='r comment'>%x <td class=comment>%s\n", 
                b->getFlags(), b->getFlagsString().c_str()); html += buf;

        if( name.find("Zone") != string::npos){
            sprintf(buf, "<tr><th>zone flags <td class='r comment'>%x <td class=comment>%s\n", 
                    b->zoneFlags(), b->zoneFlagsString().c_str()); html += buf;
        }

        sprintf(buf, "<tr><th>ptr <td class='r ptr'><a href='/hexdump?offset=%p&size=%d&width=4'>%p</a>\n", 
                b, Building::RECORD_SIZE, b
                ); html += buf;

        html += "</table>\n";

        // items in building

        WearingVector*wv = b->getItems();
        if( wv && wv->size() > 0){
            html += "<table class='wv sortable'>\n";
            html += "<tr><th>item <th class=sorttable_numeric>value\n";
        
            WearingVector::iterator itr;
            for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                html += HTML::Item((*itr)->item);
                sprintf(buf, "<td class='r flags'>%x", (*itr)->item->getFlags()); html+=buf;
            }

            html += "</table>\n";
        }

        html += "</div>\n";
        return html;
    }

    string index(){
        string html;
        char buf[0x1000];
        BuildingsVector *v = Building::getVector();

        if( !v ){
            return "<div class=error>NULL buildings_vector</div>";
        }
    
        sprintf(buf, "<h1>%d buildings</h1>\n", v->size()); html += buf;

        html += 
            "<table class='buildings sortable'>\n"
                "<tr>"
                    "<th>name"
                    "<th class=sorttable_numeric>value"
                    "<th class=sorttable_numeric>items"
                    "<th>flags"
            "\n";

        for( int i=0; i<v->size(); i++){
            Building *bld = v->at(i);
            WearingVector*wv = bld->getItems();
            sprintf(buf, 
                    "<tr>"
                        "<td>%s"
                        "<td class=r>%d<span class=currency>&#9788;</span>"
                        "<td class=r>%d"
                        "\n",
                    link_to_building(bld),
                    bld->getValue(),
                    wv ? wv->size() : 0
                    ); html += buf;

            if(strstr(buf, "Zone")){
                html += "<td class='flags'>";
                html += bld->zoneFlagsString();
            }
        }

        html += "</table>\n";

        return html;
    }
};
