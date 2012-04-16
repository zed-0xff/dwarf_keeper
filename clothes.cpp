#include "common.h"
#include <iostream>

static const char* CLOTH_TYPES[] = {
    "armor", "braies", "breastplate", "cap", "cape", "chausse", "cloak", "coat", "dress", "face veil", "gauntlet", "glove", "greaves", "head veil", "headscarf", "helm", "high boot", "hood", "leggings", "loincloth", "long skirt", "low boot", "mail shirt", "mask", "mitten", "robe", "sandal", "shirt", "shoe", "short skirt", "skirt", "sock", "thong", "toga", "trousers", "tunic", "turban", "vest"
};

// http://dwarffortresswiki.org/index.php/40d:Clothing#Layering_of_clothing

class Clothes{
    map<Item*, Dwarf*>  items2dwarves;
    //set<void*> vtables; // vtables of clothing-only items, i.e. all from CLOTH_TYPES list
    vector<void*>vtables;  // using vector instead of set saves ~300ms total
    //map<string, void*> types2vtables;
    map<void*, string> items2basenames;

    public:
    int want_free, want_owned;
    vector<string> want_types;

    Clothes(){
      // fill a pItem->pDwarf map

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

    enum count_type_t { COUNT_ALL, COUNT_FREE, COUNT_OWNED };

    string count_by_names(count_type_t ct, const char* name0, ...){
        ItemsVector*v = Item::getVector();
        va_list ap; va_start(ap, name0);
        void *pvtbl = NULL;
        int i, l, cnt_free = 0, cnt_owned = 0;
        string item_name,rs;
        char buf[0x20];
        const char*p;
        map<Item*,Dwarf*>::iterator i2d_it;

        rs = "\n<td class=r title='";
        for(va_start(ap,name0), p = name0; p; p = va_arg(ap, const char*)){
            if( p != name0) rs += ", ";
            l = strlen(p);
            for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
                if(is_clothes_vtable((*itr)->pvtbl) && check_item_type(*itr,p,l)){
                    if(items2dwarves.find(*itr) != items2dwarves.end())
                        cnt_owned++;
                    else
                        cnt_free++;
                }
            }
            rs += p;
        }
        va_end(ap);

        sprintf(buf,"'><a href='?"); rs += buf;

        for(va_start(ap,name0), p=name0; p; p = va_arg(ap, const char*)){
            if( p != name0) rs += "&";
            rs += "t=";
            rs += url_escape(p);
        }
        va_end(ap);

        sprintf(buf,"'>%d</a>",cnt); rs += buf;

        return rs;
    }

    public:

    string counts_table(count_type_t ct){
        string html;
        html.reserve(4*1024);

        html += "\n\n<table class='t1 clothes-counts'>";
        html += "\n<tr><th>part \\ layer <th>UNDER <th>OVER <th>ARMOR <th>COVER";

        html += "\n<tr><th>Headwear";

        html += count_by_names(ct,"mask", "face veil", NULL);
        html += count_by_names(ct,"cap", "turban", "head veil", "headscarf", NULL);
        html += count_by_names(ct,"helm", NULL);
        html += count_by_names(ct,"hood", NULL);

        html += "\n<tr><th>Bodywear";
        html += count_by_names(ct,"shirt", "tunic", "dress", NULL);
        html += count_by_names(ct,"coat", "toga", "vest", "robe", NULL);
        html += count_by_names(ct,"breastplate", "mail shirt", "armor", NULL);
        html += count_by_names(ct,"cloak", "cape", NULL);

        html += "\n<tr><th>Handwear";
        html += count_by_names(ct,"glove", NULL);
        html += "<td class='na' title='n/a'>-";
        html += count_by_names(ct,"gauntlet", NULL);
        html += count_by_names(ct,"mitten", NULL);

        html += "\n<tr><th>Legwear";
        html += count_by_names(ct,"loincloth", "thong", "braies", NULL);
        html += count_by_names(ct,"trousers", "skirt", "short skirt", "long skirt", NULL);
        html += count_by_names(ct,"greaves", "leggings", NULL);
        html += "<td class='na' title='n/a'>-";

        html += "\n<tr><th>Footwear";
        html += count_by_names(ct,"chausse", "sock", NULL);
        html += count_by_names(ct,"shoe", "sandal", NULL);
        html += count_by_names(ct,"high boot", "low boot", NULL);
        html += "<td class='na' title='n/a'>-";

        // XXX raw/item_shoes.txt states that "high boot" & "low boot" are in "OVER" layer
        // XXX wiki says they're in "ARMOR" layer

        html += "\n</table>";
        return html;
    }

    string to_html(){
      string html; html.reserve(10*1024);
      char buf[0x200];

      BENCH_START;
      html += counts_table(COUNT_FREE);
      html += counts_table(COUNT_OWNED);
      html += counts_table(COUNT_ALL);
      BENCH_END("counts_table() x 3");

      if(want_types.size() == 0){
          return html;
      }

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

              html += HTML::Item(*itr);
              html += "<td>";
          }

          nItems++;

#ifdef DEBUG
          sprintf(buf,"<td class=r><a class=ptr href='/hexdump?offset=%p&size=%d'>%p</a>", *itr, Item::SIZE, *itr);
          html += buf;
#endif
      }
      html += "</table>";

      sprintf(buf, "<h1>Items (%d)</h1>\n", nItems);
      return buf + html;
    }
};
