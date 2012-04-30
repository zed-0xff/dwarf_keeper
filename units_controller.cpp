#include "screen.cpp"
#include "controller.cpp"

class UnitsController : Controller {
    HTTPRequest* request;
    string want_grep;
    int race_filter;

    public:

    int resp_code;

    UnitsController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
        want_grep = request->get_string("grep","");

        if( request->url_starts_with("/dwarves") ){
            race_filter = RACE_DWARF;
        } else {
            race_filter = request->get_int("race",-1);
        }
    }

    string to_html(){
        int id = request->get_int("id",0);
        if(id){
            Unit *pc = Unit::find(id);
            if(pc){
                return show(pc);
            } else {
                resp_code = MHD_HTTP_NOT_FOUND;
                return "Not Found";
            }
        } else if( strstr(request->url, "/attr") ){
            return attributes();
        } else {
            return list();
        }
    }

    private:

    string attributes(){
        string html;
        int idx = 0;
        char buf[0x1000];
        bool was_titles = false;

        html += "<table class='units sortable'>\n";

        while(Unit* pc=Unit::getNext(&idx, race_filter)){
            PhysAttrsVector pav = pc->getPhysAttrs();
            if( pav.size() > 0 ){
                if(!was_titles){
                    html += "\n<tr><th>name";
                    for(int i=0;i<pav.size();i++){
                        sprintf(buf, "<th>%s", pav[i].name);
                        html += buf;
                    }
                    was_titles = true;
                }
                html += "\n<tr><td>";
                html += link_to_unit(pc);
                for(int i=0;i<pav.size();i++){
                    sprintf(buf, "<td class=r>%d", pav[i].value - pav[i].sub);
                    html += buf;
                }
            }
        }

        html += "</table>\n";

        return html;
    }

    // show one unit
    string show(Unit *pc){
        char buf[0x200];
        string html;
        
        html += "<div id=dwarf>\n";
        html += "<h1>" + pc->getName() + "</h1>\n";

        //sprintf(buf, "<div id=happiness>%d</div>\n", pc->getHappiness()); html += buf;
        Coords c = pc->getCoords();
        sprintf(buf,
                "<table class=tools>"
                    "<tr id='unit_%d'>"
                        "<td>"
                            "<div class=crosshair></div>"
                        "<td title='Happiness' class=happiness>%d"
                        "<td title='flags' class=flags>%x"
                        "<td title='coords' class=flags>(%d,%d,%d)"
                "</table>\n",
                pc->getId(), pc->getHappiness(), pc->getFlags(),
                c.x, c.y, c.z
        ); html += buf;

        html += "<div class=tables>\n";

        // physical attributes

        PhysAttrsVector pav = pc->getPhysAttrs();
        if( pav.size() > 0 ){
            html += "<table class='t1 phys_attrs'>\n";
            for(int i=0;i<pav.size();i++){
                sprintf(buf, "<tr title='%s'><td>%s <td class=r>%d <td class=r>%d\n", 
                        pav[i].name,
                        pav[i].name,
                        pav[i].value,
                        pav[i].sub
                        );
                html += buf;
            }
            html += "</table>\n";
        }

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

        html += "</div>\n";

        // thoughts

        html += "<div class=thoughts>" + pc->getThoughts() + "</div>\n";
        html += "</div>\n"; // div id=dwarf

        return html;
    }

    // list of units
    string list(){
        string html,s;
        char buf[0x1000];

        html += "<div id=units>\n";

        html += 
            "<form>"
                "<input type=text name=grep>"
                "<input type=submit value='grep info'>"
            "</form>\n";

        html += "<table class='dwarves sortable'>\n";
        html += "<tr>"
            "<th>name "
            "<th>profession "
            "<th title='number of items weared'>items "
            "<th class=sorttable_numeric title='total items value'>value "
            "<th class=sorttable_numeric title='greater is better'>happiness "
            "<th class=flags>flags"
            "\n";

        int idx = 0, nDwarves = 0;

        while(Unit* pc=Unit::getNext(&idx, race_filter)){
            s = pc->getName();

            if( !want_grep.empty() ){
                // grep unit description for substring
                if( pc->getThoughts().find(want_grep) == string::npos ) continue;
            }

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
        html += "</table>\n";

        sprintf(buf, "<h1>%s (%d)</h1>\n", race_filter == RACE_DWARF ? "Dwarves" : "Units" ,nDwarves);
        html = buf + html;

        html += "</div>\n";

        return html;
    }
};
