#include "common.h"
#include "screen.cpp"

class UnitsController {
    HTTPRequest* request;

    public:

    int resp_code;

    UnitsController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
    }

    string to_html(){
        int id = request->get_int("id",0);
        if(id){
            Unit *pc = Unit::find(id);
            if(pc){
                if( request->get_int("center",0) == 1 ){
                    int x=0, y=0, z=0;
                    pc->getCoords(&x,&y,&z);

                    // XXX HACK
                    *((int*)0x1563A3C) = -30000;
                    *((int*)0x1563A48) = -30000;
                    *((uint16_t*)0x156A97C) = 0x17;
                    *((int*)0x1563B5C) = -1;
                    *((int*)0x1563B68) = 0;
                    *((uint8_t*)0x16FEA54) = 1;
                    *((uint8_t*)0x16FEA55) = 1;

                    Screen::enumerate();
                    Screen::moveTo(x,y,z);
                    ((func_t_i)(FOO_FUNC))(pc->getId());

                    return "OK";
                } else {
                    return show(pc);
                }
            } else {
                resp_code = MHD_HTTP_NOT_FOUND;
                return "Not Found";
            }
        } else {
            return list();
        }
    }

    // show one unit
    string show(Unit *pc){
        char buf[0x200];
        string html;
        
        html += "<div id=dwarf>\n";
        html += "<h1>" + pc->getName() + "</h1>\n";

        //sprintf(buf, "<div id=happiness>%d</div>\n", pc->getHappiness()); html += buf;
        sprintf(buf,
                "<table class=tools>"
                    "<tr id='unit_%d'>"
                        "<td>"
                            "<div class=crosshair></div>"
                        "<td title='Happiness' class=happiness>%d"
                "</table>\n",
                pc->getId(), pc->getHappiness()
        ); html += buf;

        // wearings

        WearingVector*wv = pc->getWear();
        if(wv && wv->size() > 0){
            html += "<table class='items sortable'>\n";
            html += "<tr><th>item <th class=sorttable_numeric>value\n";

            WearingVector::iterator itr;
            for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                html += HTML::Item((*itr)->item);
            }

            html += "</table>\n";
        }

        // skills

        SkillsVector* psv = pc->getSoul()->getSkillsVector();
        if(psv && psv->size() > 0){
            html += "<table id=skills class='sortable skills'>\n";
            html += "<tr><th class=sorttable_numeric>level<th>skill\n";
            for(SkillsVector::iterator it=psv->begin(); it<psv->end(); it++){
                const char*type = NULL;
                switch((*it)->getType()){
                    case Skill::TYPE_COMBAT:
                        type = "st_combat";
                        break;
                    case Skill::TYPE_LABOR:
                        type = "st_labor";
                        break;
                    case Skill::TYPE_MISC:
                        type = "st_misc";
                        break;
                }
                sprintf(buf, "<tr class='%s'><td class=skill_level><tt>%2d</tt> %s <td class=skill_name>%s \n",
                        type ? type : "",
                        (*it)->getLevel(),
                        (*it)->levelString().c_str(),
                        (*it)->nameString(pc->getRace(), pc->getSex()).c_str()
                        ); html+=buf;
            }
            html += "</table>\n";
        }


        html += "<div class=thoughts>" + pc->getThoughts() + "</div>\n";
        html += "</div>\n"; // div id=dwarf

        return html;
    }

    // list of units
    string list(){
        string html,s;
        char buf[0x1000];

        html += "<table class='dwarves sortable'>";
        html += "<tr>"
            "<th>name "
            "<th>profession "
            "<th title='number of items weared'>items "
            "<th class=sorttable_numeric title='total items value'>value "
            "<th class=sorttable_numeric title='greater is better'>happiness"
            "\n";
        html += "<th class=flags>flags";

        int idx = 0, nDwarves = 0, race_filter;

        if( request->url_match("/dwarves") ){
            race_filter = RACE_DWARF;
        } else {
            race_filter = request->get_int("race",-1);
        }

        while(Unit* pc=Unit::getNext(&idx, race_filter)){
            s = pc->getName();

            nDwarves++;

            // dwarven babies have no useful info
            if(s.find(", Dwarven Baby") != string::npos) continue;

            if(!str_replace(s, ", ", "</a></td><td class=prof>")){
                // no profession
                s += "</a></td><td class=prof>";
            }

            sprintf(buf, 
                    "<tr id=\"unit_%d\">"
                        "<td>"
                            "<div class=crosshair></div>"
                            "<a href='?id=%d'>%s</a>", 
                    pc->getId(),
                    pc->getId(),
                    s.c_str());
            html += buf;

            int nItems = 0, totalValue = 0;
            WearingVector*wv = pc->getWear();
            WearingVector::iterator itr;
            for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                Item* pItem = (*itr)->item;
                totalValue += pItem->getValue();
                nItems++;
            }
            sprintf(buf, "<td class=r>%d</td><td class=r>%d<span class=currency>&#9788;</span></td>", nItems, totalValue);
            html += buf;

            sprintf(buf, "<td class=r>%d</td>", pc->getHappiness());
            html += buf;

            sprintf(buf, "<td class='flags r'>%x</td>", pc->getFlags());
            html += buf;

            html += "</tr>\n";
        }
        html += "</table>";

        sprintf(buf, "<h1>%s (%d)</h1>\n", race_filter == RACE_DWARF ? "Dwarves" : "Units" ,nDwarves);
        html = buf + html;

        return html;
    }
};
