#include "common.h"
#include <iostream>

#define DEBUG

#define GAUNTLET    0x1d0000
#define GLOVE       0x1d0001
#define MITTEN      0x1d0002

enum layer_t     { LAYER_UNDER=0, LAYER_OVER, LAYER_ARMOR, LAYER_COVER };
enum item_type_t { ITEM_ARMOR=0x19, ITEM_SHOES=0x1a, ITEM_HELM=0x1c, ITEM_GLOVES=0x1d, ITEM_PANTS=0x3b };

struct ClothType {
    const char* name;
    layer_t     layer;
    item_type_t type;
    uint16_t    subtype;
    uint16_t    flags;

    uint32_t inline full_id(){
        return (type<<16) + subtype;
    }
};

#define FLAG_CRAFTABLE 1

static ClothType clothtypes[] = {
    {"breastplate", LAYER_ARMOR, ITEM_ARMOR ,  0, FLAG_CRAFTABLE },
    {"mail shirt" , LAYER_ARMOR, ITEM_ARMOR ,  1, FLAG_CRAFTABLE },
    {"armor"      , LAYER_ARMOR, ITEM_ARMOR ,  2, FLAG_CRAFTABLE },
    {"coat"       , LAYER_OVER , ITEM_ARMOR ,  3, 0 },
    {"shirt"      , LAYER_UNDER, ITEM_ARMOR ,  4, FLAG_CRAFTABLE },
    {"cloak"      , LAYER_COVER, ITEM_ARMOR ,  5, FLAG_CRAFTABLE },
    {"tunic"      , LAYER_UNDER, ITEM_ARMOR ,  6, 0 },
    {"toga"       , LAYER_OVER , ITEM_ARMOR ,  7, 0 },
    {"cape"       , LAYER_COVER, ITEM_ARMOR ,  8, 0 },
    {"vest"       , LAYER_OVER , ITEM_ARMOR ,  9, 0 },
    {"dress"      , LAYER_UNDER, ITEM_ARMOR , 10, FLAG_CRAFTABLE },
    {"robe"       , LAYER_OVER , ITEM_ARMOR , 11, FLAG_CRAFTABLE },
    {"gauntlet"   , LAYER_ARMOR, ITEM_GLOVES,  0, FLAG_CRAFTABLE },
    {"glove"      , LAYER_UNDER, ITEM_GLOVES,  1, FLAG_CRAFTABLE },
    {"mitten"     , LAYER_COVER, ITEM_GLOVES,  2, FLAG_CRAFTABLE },
    {"helm"       , LAYER_ARMOR, ITEM_HELM  ,  0, FLAG_CRAFTABLE },
    {"cap"        , LAYER_OVER , ITEM_HELM  ,  1, FLAG_CRAFTABLE },
    {"hood"       , LAYER_COVER, ITEM_HELM  ,  2, FLAG_CRAFTABLE },
    {"turban"     , LAYER_OVER , ITEM_HELM  ,  3, 0 },
    {"mask"       , LAYER_UNDER, ITEM_HELM  ,  4, 0 },
    {"head veil"  , LAYER_OVER , ITEM_HELM  ,  5, 0 },
    {"face veil"  , LAYER_UNDER, ITEM_HELM  ,  6, 0 },
    {"headscarf"  , LAYER_OVER , ITEM_HELM  ,  7, 0 },
    {"trousers"   , LAYER_OVER , ITEM_PANTS ,  0, FLAG_CRAFTABLE },
    {"greaves"    , LAYER_ARMOR, ITEM_PANTS ,  1, FLAG_CRAFTABLE },
    {"leggings"   , LAYER_ARMOR, ITEM_PANTS ,  2, FLAG_CRAFTABLE },
    {"loincloth"  , LAYER_UNDER, ITEM_PANTS ,  3, 0 },
    {"thong"      , LAYER_UNDER, ITEM_PANTS ,  4, 0 },
    {"skirt"      , LAYER_OVER , ITEM_PANTS ,  5, 0 },
    {"short skirt", LAYER_OVER , ITEM_PANTS ,  6, 0 },
    {"long skirt" , LAYER_OVER , ITEM_PANTS ,  7, 0 },
    {"braies"     , LAYER_UNDER, ITEM_PANTS ,  8, 0 },
    {"shoe"       , LAYER_OVER , ITEM_SHOES ,  0, FLAG_CRAFTABLE },
    {"high boot"  , LAYER_OVER , ITEM_SHOES ,  1, FLAG_CRAFTABLE },
    {"low boot"   , LAYER_OVER , ITEM_SHOES ,  2, FLAG_CRAFTABLE },
    {"sandal"     , LAYER_OVER , ITEM_SHOES ,  3, 0 },
    {"chausse"    , LAYER_UNDER, ITEM_SHOES ,  4, 0 },
    {"sock"       , LAYER_UNDER, ITEM_SHOES ,  5, FLAG_CRAFTABLE },
};

#define MIN_FREE_CRAFTABLES 5

// http://dwarffortresswiki.org/index.php/40d:Clothing#Layering_of_clothing

class Clothes{
    map<Item*, Dwarf*>  items2dwarves;
    vector<uint32_t>    want_types;

    public:
    int want_free, want_owned, want_stats, want_unusable, free_max_wear;

    Clothes(){
      // fill a pItem->pDwarf map

      want_stats  = 1;
      want_owned  = 1;
      want_free   = 1;
      want_unusable = 0;
      free_max_wear = Item::WEAR_OK;

      BENCH_START;
      int idx=0;
      while(Dwarf* pDwarf=Dwarf::getNext(&idx)){
              WearingVector*wv = pDwarf->getWear();
              WearingVector::iterator itr;
              for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                  items2dwarves.insert(pair<Item*,Dwarf*>((*itr)->item, pDwarf));
              }
      }
      BENCH_END("filling items2dwarves");
    }

    void setWantTypes(vector<string> typeNames){
        want_types.clear();
        for(int i=0; i<typeNames.size(); i++){
            for(int j=0; j<sizeof(clothtypes)/sizeof(clothtypes[0]); j++){
                if(!typeNames[i].compare(clothtypes[j].name)){
                    want_types.push_back(clothtypes[j].full_id());
                    break;
                }
            }
        }
    }

    private:

    bool is_clothing(Item*pItem){
        switch(pItem->getTypeId()){
            case ITEM_GLOVES:
            case ITEM_ARMOR:
            case ITEM_SHOES:
            case ITEM_PANTS:
            case ITEM_HELM:
                return true;
        }
        return false;
    }

    int count_unusable(){
        ItemsVector*v = Item::getVector();
        int cnt = 0;

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            if( is_clothing(*itr) && (*itr)->getSize() != Item::SIZE_OK )
                cnt++;
        }
        return cnt;
    }

    void count_by_type( item_type_t type, int subtype, int*cnt_owned, int*cnt_free ){
        ItemsVector*v = Item::getVector();

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            if(
                    (*itr)->getTypeId()    == type &&
                    (*itr)->getSubTypeId() == subtype &&
                    (*itr)->getSize()      == Item::SIZE_OK
            ){
                if(items2dwarves.find(*itr) != items2dwarves.end())
                    (*cnt_owned)++;
                else if((*itr)->getWear() <= free_max_wear )
                    (*cnt_free)++;
            }
        }
    }

    string count_td( layer_t layer, item_type_t type){
        string html;
        ClothType*pct;
        int cnt_owned = 0, cnt_free = 0;
        char buf[0x200];
        vector <const char*> item_names;
        bool found = false;
        bool craftable = false;

        for(int i=0; i<sizeof(clothtypes)/sizeof(clothtypes[0]); i++){
            pct = &clothtypes[i];
            if( pct->layer == layer && pct->type == type ){
                count_by_type(pct->type, pct->subtype, &cnt_owned, &cnt_free);
                item_names.push_back(pct->name);
                if(pct->flags & FLAG_CRAFTABLE) craftable = true;
                found = true;
            }
        }

        // no itemtypes have both requested layer and type => n/a
        if(!found) return "\n<td class='na' title='n/a'>-";

        string url = "?t=";

        html += "\n<td class=r title='";
        for(int i=0; i<item_names.size(); i++){
            if( i>0 ){
                html += ", ";
                url  += "&t=";
            }
            html += item_names[i];
            url  += item_names[i];
        }
        html += "'>";


        if( craftable && cnt_free < MIN_FREE_CRAFTABLES ){
            // looks like we have a shortage
            sprintf(buf,"<a class='cnt-free shortage'"
                    " title='low count of free items!'"
                    " href='%s&owned=0&free=1'>%d</a>", url.c_str(), cnt_free);  html += buf;
        } else {
            sprintf(buf,"<a class=cnt-free  href='%s&owned=0&free=1'>%d</a>", url.c_str(), cnt_free);  html += buf;
        }

        sprintf(buf,"<a class=cnt-owned href='%s&owned=1&free=0'>%d</a>", url.c_str(), cnt_owned); html += buf;

        return html;
    }

    string count_row( const char* title, item_type_t type ){
        string html;
        html += "\n<tr><th>";
        html += title;

        html += count_td( LAYER_UNDER, type );
        html += count_td( LAYER_OVER,  type );
        html += count_td( LAYER_ARMOR, type );
        html += count_td( LAYER_COVER, type );

        return html;
    }

    public:

    string counts_table(){
        int ct = 0; // unused
        string html;
        html.reserve(4*1024);

//        html += "toggle: <button id=btn-clothes-owned>owned</button>";
//        html += "<button id=btn-clothes-free>free</button>\n";

        html += "<button id=btn-clothes-toggle>toggle</button>";

        html += "<h2 class=cnt-free><span class=important>FREE</span> clothes counts:</h2>\n";
        html += "<h2 class=cnt-owned><span class=important>OWNED</span> clothes counts:</h2>\n";

        html += "\n<table class='t1 clothes-counts'>";
        html += "\n<tr><th>part \\ layer <th>UNDER <th>OVER <th>ARMOR <th>COVER";

        html += count_row( "Headwear", ITEM_HELM   );
        html += count_row( "Bodywear", ITEM_ARMOR  );
        html += count_row( "Handwear", ITEM_GLOVES );
        html += count_row( "Legwear",  ITEM_PANTS  );
        html += count_row( "Footwear", ITEM_SHOES  );

        int nUnusable = count_unusable();
        if( nUnusable > 0 ){
            char buf[0x200];
            sprintf(buf,"<tr><td class='r unusable' colspan=10>"
                    "+<a href='?unusable=1&free=1&owned=1'>%d</a> unusable items "
                    "<div class=comment>(sell them or melt them)</div>", nUnusable);
            html += buf;
        }

        // XXX raw/item_shoes.txt states that "high boot" & "low boot" are in "OVER" layer
        // XXX wiki says they're in "ARMOR" layer

        html += "\n</table>";
        return html;
    }

    string items_tbl(){
      string html; html.reserve(10*1024);
      char buf[0x200];

      html += "\n\n<table class='items sortable'>\n";
      html += "<tr><th>item <th class=sorttable_numeric>value <th>owner\n";

      // find items of type in vtables set
      ItemsVector*v = Item::getVector();
      ItemsVector::iterator itr;
      int nItems = 0;
      for ( itr = v->begin(); itr < v->end(); ++itr ) {
          if(!is_clothing(*itr)) continue;

          if(want_types.size() > 0){
              bool ok = false;
              uint32_t itfid = (*itr)->getFullId(); // item type full id
              for(int i=0; i<want_types.size(); i++){
                if(want_types[i] == itfid){
                    ok = true;
                    break;
                }
              }
              if(!ok) continue; // not in want_types
          }

          if(want_unusable){
              if((*itr)->getSize() == Item::SIZE_OK) continue;
          } else {
              if((*itr)->getSize() != Item::SIZE_OK) continue;
          }

          map<Item*,Dwarf*>::iterator i2d_it = items2dwarves.find(*itr);
          if(i2d_it != items2dwarves.end()){
              // item belongs to someone
              if(!want_owned) continue;

              html += HTML::Item(*itr);
              sprintf(buf, "<td class=owner><a href='/dwarves/%04x'>%s</a>", 
                      i2d_it->second->getId(),
                      i2d_it->second->getName().c_str());
              html += buf;
          } else {
              // item is free
              if(!want_free) continue;
              if((*itr)->getWear() > free_max_wear ) continue;

              html += HTML::Item(*itr);
              html += "<td>";
          }

          nItems++;

#ifdef DEBUG
          sprintf(buf,
                  "<td class=r>"
                  "<a class=ptr href='/hexdump?offset=%p&size=%d&width=4&title=%s'>"
                  "%p"
                  "</a>"
                  "<td class=r>%x:%x",
                  *itr, Item::RECORD_SIZE, url_escape((*itr)->getName()).c_str(), *itr,
                  (*itr)->getTypeId(), (*itr)->getSubTypeId() );
          html += buf;
#endif
      }
      html += "</table>";

      string s; s.reserve(html.size() + 100);
      sprintf(buf, "<h2>%d", nItems);
      s = buf;

      if( want_owned && !want_free ){
          s += " <span class=important>OWNED</span>";
      } else if( want_free && !want_owned){
          s += " <span class=important>FREE</span>";
      }

      s += " items of type: ";

      for(int i=0; i<want_types.size(); i++){
          for(int j=0; j<sizeof(clothtypes)/sizeof(clothtypes[0]); j++){
              if(want_types[i] == clothtypes[j].full_id()){
                  if( i>0 ) s += ", ";
                  s += "<span class=important>" + html_escape(clothtypes[j].name) + "</span>";
                  break;
              }
          }
      }
      s += "</h2>\n";
      s += html;

      return s;
    }

    string to_html(){
      string html; html.reserve(10*1024);
      char buf[0x200];

      if(want_stats){
          html += "<h1>Clothes</h1>\n";

          BENCH_START;
          html += counts_table();
          BENCH_END("counts_table()");

          html += "<div id='clothes-tbl-place'>\n";
      }

      if(want_types.size() > 0 || want_unusable){
          BENCH_START;
          html += items_tbl();
          BENCH_END("items tbl");
      }

      if(want_stats){
          html += "</div>\n";
      }

      return html;
    }
};
