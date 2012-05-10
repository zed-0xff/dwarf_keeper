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
            race_filter = Unit::getDwarfRace();
        } else {
            race_filter = request->get_int("race",-1);
        }
    }

    string to_html(){
        int id = request->get_int("id",0);
        if(id){
            Unit *unit = Unit::find(id);
            if(unit){
                return show(unit);
            } else {
                resp_code = MHD_HTTP_NOT_FOUND;
                return "Not Found";
            }
        } else if( strstr(request->url, "/attr") ){
            return attributes();
        } else if( strstr(request->url, "/skills") ){
            return skills();
        } else {
            return list();
        }
    }

    private:

    string skills(){
        string html;
        int idx = 0;
        char buf[0x1000];

        int want_combat = request->get_string("combat", "") == "on";
        int want_labor  = request->get_string("labor", "") == "on";
        int want_misc   = request->get_string("misc", "") == "on";
        int min_level   = request->get_int("min_level",1);

        if( want_combat + want_labor + want_misc == 0){
            want_combat = want_labor = 1;
        }

        sprintf(buf,
                "<form>"
                "<div id='skills-checkboxes'>\n"
                    "<label for='min_level'>min level: </label><input id=min_level name=min_level size=3 value=%d>\n"
                    "<input type=checkbox id=ch_c name='combat' %s><label for='ch_c'>Combat</label>\n"
                    "<input type=checkbox id=ch_l name='labor'  %s><label for='ch_l'>Labor </label>\n"
                    "<input type=checkbox id=ch_m name='misc'   %s><label for='ch_m'>Misc  </label>\n"
                    "<input type=submit>"
                "</div>\n"
                "</form>\n",
                min_level,
                want_combat ? "CHECKED" : "",
                want_labor  ? "CHECKED" : "",
                want_misc   ? "CHECKED" : ""
               ); html += buf;

        html += "<table id=skills class='sortable skills'>\n";
        html += 
            "<tr>"
                "<th>unit"
                "<th class=sorttable_numeric>level"
                "<th>skill\n";

        while(Unit* unit=Unit::getNext(&idx, race_filter)){
            SkillsVector* psv = unit->getSoul()->getSkillsVector();
            if(psv && psv->size() > 0){
                for(SkillsVector::iterator it=psv->begin(); it<psv->end(); it++){
                    Skill* skill = *it;
                    if( skill->getLevel() < min_level ) continue;

                    const char*type = NULL;
                    switch(skill->getType()){
                        case Skill::TYPE_COMBAT:
                            type = "st_combat";
                            if(!want_combat) continue;
                            break;
                        case Skill::TYPE_LABOR:
                            type = "st_labor";
                            if(!want_labor) continue;
                            break;
                        case Skill::TYPE_MISC:
                            type = "st_misc";
                            if(!want_misc) continue;
                            break;
                    }

                    string name = unit->getName();
                    int pos = name.find(", ");
                    if( pos != string::npos ) name.erase(pos); // don't show unit main profession in unit name

                    sprintf(buf, 
                            "<tr id='unit_%d' class='%s'>"
                                "<td><div class=crosshair></div>%s"
                                "<td class=skill_level><tt>%2d</tt> %s "
                                "<td class=skill_name>%s \n",
                            unit->getId(),
                            type ? type : "",
                            link_to_unit(unit, name.c_str()),
                            skill->getLevel(),
                            skill->levelString().c_str(),
                            skill->nameString(unit->getRace(), unit->getSex()).c_str()
                            ); html+=buf;
                }
            }
        }

        html += "</table>\n";

        return html;
    }

    string attributes(){
        string html;
        int idx = 0;
        char buf[0x1000];
        bool was_titles = false;

        html += "<table class='units sortable'>\n";

        while(Unit* unit=Unit::getNext(&idx, race_filter)){
            PhysAttrsVector pav = unit->getPhysAttrs();
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
                html += link_to_unit(unit);
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
    string show(Unit *unit){
        char buf[0x200];
        string html;
        
        html += "<div id=dwarf>\n";
        html += "<h1>" + unit->getName() + "</h1>\n";

        //sprintf(buf, "<div id=happiness>%d</div>\n", unit->getHappiness()); html += buf;
        Coords c = unit->getCoords();
        sprintf(buf,
                "<table class=tools>"
                    "<tr id='unit_%d'>"
                        "<td>"
                            "<div class=crosshair></div>"
                        "<td title='Happiness' class=happiness>%d"
                        "<td title='flags' class=flags>%x"
                        "<td title='coords' class=flags>(%d,%d,%d)"
                "</table>\n",
                unit->getId(), unit->getHappiness(), unit->getFlags(),
                c.x, c.y, c.z
        ); html += buf;

        html += "<div class=tables>\n";

        // physical attributes

        PhysAttrsVector pav = unit->getPhysAttrs();
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

        WearingVector*wv = unit->getWear();
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

        SkillsVector* psv = unit->getSoul()->getSkillsVector();
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
                        (*it)->nameString(unit->getRace(), unit->getSex()).c_str()
                        ); html+=buf;
            }
            html += "</table>\n";
        }

        html += "</div>\n";

        // thoughts

        html += "<div class=thoughts>" + unit->getThoughts() + "</div>\n";
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

        int idx = 0, nUnits = 0;

        while(Unit* unit=Unit::getNext(&idx, race_filter)){
            s = unit->getName();

            if( !want_grep.empty() ){
                // grep unit description for substring
                if( unit->getThoughts().find(want_grep) == string::npos ) continue;
            }

            nUnits++;

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
                    unit->getId(),
                    unit->getId(),
                    s.c_str());
            html += buf;

            int nItems = 0, totalValue = 0;
            WearingVector*wv = unit->getWear();
            WearingVector::iterator itr;
            for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                Item* pItem = (*itr)->item;
                totalValue += pItem->getValue();
                nItems++;
            }
            sprintf(buf, "<td class=r>%d</td><td class=r>%d<span class=currency>&#9788;</span></td>", nItems, totalValue);
            html += buf;

            sprintf(buf, "<td class=r>%d</td>", unit->getHappiness());
            html += buf;

            sprintf(buf, "<td class='flags r'>%x</td>", unit->getFlags());
            html += buf;

            html += "</tr>\n";
        }
        html += "</table>\n";

        sprintf(buf, "<h1>%s (%d)</h1>\n", race_filter == Unit::getDwarfRace() ? "Dwarves" : "Units" ,nUnits);
        html = buf + html;

        html += "</div>\n";

        return html;
    }
};
