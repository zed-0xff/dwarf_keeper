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
    int         subtype;
};

static ClothType clothtypes[] = {
    {"breastplate", LAYER_ARMOR, ITEM_ARMOR ,  0 },
    {"mail shirt" , LAYER_ARMOR, ITEM_ARMOR ,  1 },
    {"armor"      , LAYER_ARMOR, ITEM_ARMOR ,  2 },
    {"coat"       , LAYER_OVER , ITEM_ARMOR ,  3 },
    {"shirt"      , LAYER_UNDER, ITEM_ARMOR ,  4 },
    {"cloak"      , LAYER_COVER, ITEM_ARMOR ,  5 },
    {"tunic"      , LAYER_UNDER, ITEM_ARMOR ,  6 },
    {"toga"       , LAYER_OVER , ITEM_ARMOR ,  7 },
    {"cape"       , LAYER_COVER, ITEM_ARMOR ,  8 },
    {"vest"       , LAYER_OVER , ITEM_ARMOR ,  9 },
    {"dress"      , LAYER_UNDER, ITEM_ARMOR , 10 },
    {"robe"       , LAYER_OVER , ITEM_ARMOR , 11 },
    {"gauntlet"   , LAYER_ARMOR, ITEM_GLOVES,  0 },
    {"glove"      , LAYER_UNDER, ITEM_GLOVES,  1 },
    {"mitten"     , LAYER_COVER, ITEM_GLOVES,  2 },
    {"helm"       , LAYER_ARMOR, ITEM_HELM  ,  0 },
    {"cap"        , LAYER_OVER , ITEM_HELM  ,  1 },
    {"hood"       , LAYER_COVER, ITEM_HELM  ,  2 },
    {"turban"     , LAYER_OVER , ITEM_HELM  ,  3 },
    {"mask"       , LAYER_UNDER, ITEM_HELM  ,  4 },
    {"head veil"  , LAYER_OVER , ITEM_HELM  ,  5 },
    {"face veil"  , LAYER_UNDER, ITEM_HELM  ,  6 },
    {"headscarf"  , LAYER_OVER , ITEM_HELM  ,  7 },
    {"trousers"   , LAYER_OVER , ITEM_PANTS ,  0 },
    {"greaves"    , LAYER_ARMOR, ITEM_PANTS ,  1 },
    {"leggings"   , LAYER_ARMOR, ITEM_PANTS ,  2 },
    {"loincloth"  , LAYER_UNDER, ITEM_PANTS ,  3 },
    {"thong"      , LAYER_UNDER, ITEM_PANTS ,  4 },
    {"skirt"      , LAYER_OVER , ITEM_PANTS ,  5 },
    {"short skirt", LAYER_OVER , ITEM_PANTS ,  6 },
    {"long skirt" , LAYER_OVER , ITEM_PANTS ,  7 },
    {"braies"     , LAYER_UNDER, ITEM_PANTS ,  8 },
    {"shoe"       , LAYER_OVER , ITEM_SHOES ,  0 },
    {"high boot"  , LAYER_OVER , ITEM_SHOES ,  1 },
    {"low boot"   , LAYER_OVER , ITEM_SHOES ,  2 },
    {"sandal"     , LAYER_OVER , ITEM_SHOES ,  3 },
    {"chausse"    , LAYER_UNDER, ITEM_SHOES ,  4 },
    {"sock"       , LAYER_UNDER, ITEM_SHOES ,  5 },
};

static const char* CLOTH_TYPES[] = {
    "armor", "braies", "breastplate", "cap", "cape", "chausse", "cloak", "coat", "dress", "face veil", "gauntlet", "glove", "greaves", "head veil", "headscarf", "helm", "high boot", "hood", "leggings", "loincloth", "long skirt", "low boot", "mail shirt", "mask", "mitten", "robe", "sandal", "shirt", "shoe", "short skirt", "skirt", "sock", "thong", "toga", "trousers", "tunic", "turban", "vest"
};

static const char* CRAFTABLES[] = {
    "armor", "breastplate", "cap", "cloak", "dress", "gauntlet", "glove", "greaves", "helm", "high boot", "hood", "leggins", "low boot", "mail shirt", "mitten", "robe", "shirt", "shoe", "sock", "trousers"
};

#define MIN_FREE_CRAFTABLES 5

// http://dwarffortresswiki.org/index.php/40d:Clothing#Layering_of_clothing

class Clothes{
    map<Item*, Dwarf*>  items2dwarves;
    //set<void*> vtables; // vtables of clothing-only items, i.e. all from CLOTH_TYPES list
    vector<void*>vtables;  // using vector instead of set saves ~300ms total
    //map<string, void*> types2vtables;
    map<void*, string> items2basenames;

    public:
    int want_free, want_owned, want_stats, want_unusable, free_max_wear;
    vector<string> want_types;

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

      BENCH_START;
      // gather vtables
      set<void*> avtables, svtables;
      ItemsVector*v = Item::getVector();
      ItemsVector::iterator itr;
      for( itr = v->begin(); itr < v->end(); ++itr) {
          string item_name;
          int l;

          if(avtables.find((*itr)->pvtbl) != avtables.end()) continue; // already in all vtables
          avtables.insert((*itr)->pvtbl);

          for(int i=0; i<(sizeof(CLOTH_TYPES)/sizeof(CLOTH_TYPES[0])); i++){
              // if item_name ends on one of clothtypes[] array
              if(check_item_type(*itr, CLOTH_TYPES[i])){
                  svtables.insert((*itr)->pvtbl);
                  break;
              }
          }
      }
      BENCH_END("gathering vtables");

      vtables.reserve(svtables.size());
      for( set<void*>::iterator it = svtables.begin(); it != svtables.end(); it++){
          vtables.push_back(*it);
      }

      // caching items basenames saves ~50ms total
//      BENCH_START;
//      // map of ~20k entries
//      for( itr = v->begin(); itr < v->end(); ++itr) {
//          if(avtables.find((*itr)->pvtbl) == vtables.end()) continue; // not in vtables
//          items2basenames[*itr] = (*itr)->getBaseName(1);
//      }
//      BENCH_END("gathering basenames");
//      //printf("[d] %ld basenames\n", items2basenames.size());
    }

    private:

    bool check_item_type(Item* pItem, const char*type, int l){
        string item_name = pItem->getBaseName(1);
        return(item_name.size() > l && !strncmp(item_name.data()+item_name.size()-l, type, l));
    }
    bool check_item_type(Item* pItem, const char*type){
        return check_item_type(pItem, type, strlen(type));
    }

    bool is_clothes_vtable(void*pvtbl){
        // using operator[] instead of vector::iterator saves ~500ms total!
        for(int i=0; i<vtables.size(); i++){
            if(vtables[i] == pvtbl) return true;
        }
        return false;
    }

    int count_unusable(){
        ItemsVector*v = Item::getVector();
        int cnt = 0;

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            if( is_clothes_vtable((*itr)->pvtbl) && (*itr)->getSize() != Item::SIZE_OK )
                cnt++;
        }
        return cnt;
    }

    string count_by_names(const char* name0, ...){
        ItemsVector*v = Item::getVector();
        va_list ap; va_start(ap, name0);
        void *pvtbl = NULL;
        int i, l, cnt_free = 0, cnt_owned = 0;
        string item_name,rs;
        char buf[0x200];
        const char*p;
        map<Item*,Dwarf*>::iterator i2d_it;

        rs = "\n<td class=r title='";
        for(va_start(ap,name0), p = name0; p; p = va_arg(ap, const char*)){
            if( p != name0) rs += ", ";
            l = strlen(p);
            for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
                if(
                        is_clothes_vtable((*itr)->pvtbl) && 
                        check_item_type(*itr,p,l) &&
                        (*itr)->getSize() == Item::SIZE_OK
                ){
                    if(items2dwarves.find(*itr) != items2dwarves.end())
                        cnt_owned++;
                    else if((*itr)->getWear() <= free_max_wear )
                        cnt_free++;
                }
            }
            rs += p;
        }
        va_end(ap);

        rs += "'>"; // end of TD

        string url; // common part of url
        for(va_start(ap,name0), p=name0; p; p = va_arg(ap, const char*)){
            url += (p == name0) ? "?" : "&";
            url += "t=";
            url += url_escape(p);
        }
        va_end(ap);

        sprintf(buf,"<a class=cnt-owned href='%s&owned=1&free=0'>%d</a>", url.c_str(), cnt_owned); rs += buf;

        bool shortage = false;
        if(cnt_free < MIN_FREE_CRAFTABLES){
            for(va_start(ap,name0), p=name0; p && !shortage; p = va_arg(ap, const char*)){
                for(int i=0; i<sizeof(CRAFTABLES)/sizeof(CRAFTABLES[0]); i++){
                    if(!strcmp(CRAFTABLES[i], p)){
                        // itemtype is craftable and number of free items of type < 10
                        // looks like we have shortage on it
                        shortage = true;
                        break;
                    }
                }
            }
            va_end(ap);
        }

        if( shortage ){
            sprintf(buf,"<a class='cnt-free shortage'"
                    " title='low count of free items!'"
                    " href='%s&owned=0&free=1'>%d</a>", url.c_str(), cnt_free);  rs += buf;
        } else {
            sprintf(buf,"<a class=cnt-free  href='%s&owned=0&free=1'>%d</a>", url.c_str(), cnt_free);  rs += buf;
        }

        return rs;
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

        for(int i=0; i<sizeof(clothtypes)/sizeof(clothtypes[0]); i++){
            pct = &clothtypes[i];
            if( pct->layer == layer && pct->type == type ){
                count_by_type(pct->type, pct->subtype, &cnt_owned, &cnt_free);
                item_names.push_back(pct->name);
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

        sprintf(buf,"<a class=cnt-owned href='%s&owned=1&free=0'>%d</a>", url.c_str(), cnt_owned); html += buf;
        sprintf(buf,"<a class=cnt-free  href='%s&owned=0&free=1'>%d</a>", url.c_str(), cnt_free ); html += buf;

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
          if(!is_clothes_vtable((*itr)->pvtbl)) continue;

          if(want_types.size() > 0){
              bool ok = false;
              for(vector<string>::iterator itt = want_types.begin(); itt<want_types.end(); itt++){
                  if(check_item_type(*itr,itt->data(),itt->size())){
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
                  *itr, Item::SIZE, url_escape((*itr)->getName()).c_str(), *itr,
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

      for(vector<string>::iterator itt = want_types.begin(); itt<want_types.end(); itt++){
          if( itt != want_types.begin()) s += ", ";
          s += "<span class=important>" + html_escape(*itt) + "</span>";
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
