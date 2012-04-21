#include "common.h"

static const char* GEM_CUTS[] = { 
    "square", "oval", "tapered", "octagon", "rectangular", "round", "cushion", "emerald",
    "table", "baguette", "brilliant", "radiant", "marquise", "briolette", "trillion",
    "pear", "point", "cushion", "single"
};


class ItemsController {
    map<uint32_t, int> counts_map;
    HTTPRequest* request;

    public:

    ItemsController(HTTPRequest& req){
        request = &req;
    }

    string to_html(){
        int type_id;
        string html;

        html += "<div id=items>\n";
        html += index();

        type_id = request->get_int("t", -1);
        if(type_id != -1){
            html += items_of_type(type_id);
        }

        html += "</div>\n";
        return html;
    }

    private:

    struct count_chunk { int count; int total_value; Item*pItem; };

    string items_of_type(int type_id){
        string html, name;
        ItemsVector*v = Item::getVector();
        char buf[0x200];
        int cnt = 0;
        map <string, count_chunk> m;

        html += "<table class='items sortable'>\n"
            "<tr>"
            "<th>item"
            "<th class=sorttable_numeric title='average value'>value"
            "<th>count"
            "<th class=sorttable_numeric title='total value'>total"
            "\n";

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            if(type_id != (*itr)->getTypeId()) continue;

            name = (*itr)->getName();
            switch(type_id){
                case 1: // cut gems
                    int pos = name.find(" cut ");
                    if( pos != string::npos ){
                        name.erase(0,pos+1);
                    }

                    // "oval prases" => "cut prases"
                    for(int i=0; i<sizeof(GEM_CUTS)/sizeof(GEM_CUTS[0]); i++){
                        if( name.substr(0, strlen(GEM_CUTS[i])) == GEM_CUTS[i] ){
                            name.replace(0,strlen(GEM_CUTS[i]), "cut");
                            break;
                        }
                    }   

                    // "cut alexandrite cabochons" => "cut alexandrites"
                    if( name.size() > 10 && name.substr(name.size()-10,10) == " cabochons" ){
                        name.replace(name.size()-10, 10, "s");
                    }

                    // "cut golden beryls)" => "cut golden beryls"
                    if( name[0] != '(' && name[name.size()-1] == ')'){
                        name.erase(name.size()-1,1);
                    }
                    break;
            }
            count_chunk& cc = m[name];
            //    html += HTML::Item(*itr);
            //    cnt += 1;
            cc.count++;
            cc.total_value += (*itr)->getValue();
            cc.pItem = *itr;
            cnt++;
        }

        for(map<string,count_chunk>::iterator it = m.begin(); it != m.end(); it++){
            html += HTML::Item(it->first.c_str(), it->second.total_value / it->second.count);
            sprintf(buf, "<td class=r>%d <td class=r>%d<span class=currency>&#9788;</span>\n",
                    it->second.count,
                    it->second.total_value
                    );
            html += buf;
        }

        html += "</table>\n";

        sprintf(buf, "<h2>%d items of type %d</h2>\n", cnt, type_id);
        html = buf + html;

        return html;
    }

    string index(){
        string html;
        char buf[0x200];

        BENCH_START;
        count_by_types();
        BENCH_END("ItemsController::count_by_types");

        html += "<table class='counts-by-type sortable'>\n";
        html += "<tr> <th>type <th>count\n";

        for(map<uint32_t,int>::iterator it = counts_map.begin(); it != counts_map.end(); it++){
            sprintf(buf, "<tr><td>%s <td class=r><a href='?t=%d'>%d</a>\n", 
                    type2name(it->first), 
                    it->first, 
                    it->second);
            html += buf;
        }
        html += "</table>\n";

        return html;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    const char* type2name(int type_id){
        static char buf[0x100];
        switch(type_id){
            case  0: return "bars";
            case  1: return "small cut gems";
            case  2: return "blocks";
            case  3: return "rough gems";
            case  4: return "stones";
            case  5: return "logs";
            case  6: return "doors";
            case  7: return "floodgates";
            case  8: return "beds";
            case  9: return "thrones";
            case 17: return "barrels";
            case 25: return "bodywear";
            case 26: return "footwear";
            case 27: return "shields";
            case 28: return "headwear";
            case 29: return "handwear";
            case 30: return "boxes &amp; bags";
            case 31: return "bins";
            case 32: return "armor stands";
            case 33: return "weapon racks";
            case 34: return "cabinets";
            case 38: return "ammo";
            case 39: return "crowns";
            case 41: return "earrings";
            case 43: return "large cut gems";
            case 48: return "fish";
            case 54: return "leather";
            case 57: return "cloth";
            case 59: return "legwear";
            case 66: return "mechanisms";
            case 76: return "pipe sections";
            case 83: return "traction benches";
            case 86: return "slabs";
            case 87: return "eggs";
            case 88: return "books";
            default:
                     sprintf(buf, "%d", type_id);
                     return buf;
        }
    }

    void count_by_types(){
        ItemsVector*v = Item::getVector();

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            counts_map[(*itr)->getTypeId()]++;
        }
    }
};
