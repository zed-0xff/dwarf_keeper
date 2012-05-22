#include "common.h"
#include "soul.cpp"

// 34.07:
//static const int RACE_CAT   = 0x09e;
//static const int RACE_DUCK  = 0x0a8;
//static const int RACE_DWARF = 0x1d1;

struct Wearing {
    Item*item;
    int body_part;
};

struct Coords {
    int16_t x,y,z;
};

//static const char* PHYS_ATTR_NAMES[] = {"Strength", "Agility", "Toughness", "Endurance", "Recuperation", "Disease Resistance", "#7", "#8"};
static const char* PHYS_ATTR_NAMES[] = {"Strength", "Agility", "Toughness", "Endurance", "Recuperation", "Disease Resistance"};
#define PHYS_ATTR_COUNT (sizeof(PHYS_ATTR_NAMES)/sizeof(PHYS_ATTR_NAMES[0]))

struct PhysAttr {
    int value;
    int sub;
    const char* name;
};

class Unit;

typedef vector<Wearing*> WearingVector;
typedef vector<Unit*>    UnitsVector;
typedef vector<PhysAttr> PhysAttrsVector;

class Unit : public MemClass {
    public:

    static const int RECORD_SIZE = 0xa00;

    string getName(){
        if(GAME.unit_name_func){
            string s;
            ((unit_name_func_t)GAME.unit_name_func)(this, &s, 0);
            return cp437_to_utf8(s);
        } else {
            return "Error: unit_name_func is NULL";
        }
    }

    int getId(){
        return i(ID_OFFSET);
    }

    static const int FLAG_DEAD = 2;

    int getFlags(){
        return dw(FLAGS_OFFSET);
    }

    int getRace(){
        return i(RACE_OFFSET);
    }

    WearingVector* getWear(){
        if( GAME.unit_wearings_vector_offset){
            return (WearingVector*)checked_vector(GAME.unit_wearings_vector_offset);
        } else {
            return NULL;
        }
    }

    int getHappiness(){
        return GAME.unit_happiness_offset ? i(GAME.unit_happiness_offset) : -1;
    }

    string getThoughts(){
        if( GAME.unit_info_func ){
            string s;
            ((func_t_2_pvoids)(GAME.unit_info_func))(this, &s);
            return cp437_to_utf8(s);
        } else {
            return "Error: unit_info_func is NULL";
        }
    }

    // 0 = FEMALE, 1 = MALE
    int getSex(){ return w(SEX_OFFSET); }

    Soul* getSoul(){ 
        if( GAME.unit_soul_offset ){
            return (Soul*)(dw(GAME.unit_soul_offset)); 
        } else {
            return NULL;
        }
    } // XXX will not work on 64bit

    Coords getCoords(){
        Coords c = {-1, -1, -1};
        if( GAME.unit_coords_func ){
            ((func_t_pppp)(GAME.unit_coords_func))(this, &c.x, &c.y, &c.z);
        } else {
            c.x = w(COORD_X_OFFSET);
            c.y = w(COORD_Y_OFFSET);
            c.z = w(COORD_Z_OFFSET);
        }
        return c;
    }

    PhysAttrsVector getPhysAttrs(){
        vector<PhysAttr> r;

        if( !GAME.unit_phys_attrs_offset ){
            PhysAttr pa;
            pa.value = pa.sub = 0;
            pa.name = "GAME.unit_phys_attrs_offset is NULL";
            r.push_back(pa);
            return r;
        }

        r.reserve(PHYS_ATTR_COUNT);
        for(int i=0; i<PHYS_ATTR_COUNT; i++){
            PhysAttr pa;
            pa.value = dw( GAME.unit_phys_attrs_offset + PHYS_ATTR_SIZE*i );
            pa.sub   = dw( GAME.unit_phys_attrs_offset + PHYS_ATTR_SIZE*i + 0x10 );
            pa.name  = PHYS_ATTR_NAMES[i];
            r.push_back(pa);
        }
        return r;
    }

    //////////////////////////////////////////////////////////////////

    static Unit* find(int id){
        UnitsVector* v = (UnitsVector*)GAME.units_vector;
        if(!v) return NULL;

        for(int i=0; i<v->size(); i++){
            if(v->at(i)->getId() == id){
                return v->at(i);
            }
        }
        return NULL;
    }

    static Unit* getNext(int*idx, int race_filter = -1){
        UnitsVector* v = (UnitsVector*)GAME.units_vector;
        if(!v) return NULL;

        Unit *pc;
        while((*idx) < v->size()){
            pc = v->at((*idx)++);

            // apply race_filter
            if( race_filter != -1 && pc->getRace() != race_filter ) 
                continue;

            // skip dead units
            if((pc->getFlags() & FLAG_DEAD) != 0) 
                continue;

            return pc;
        }

        return NULL;
    }

    static int getDwarfRace(){
        static int r = 0;
        if(r) return r;

        int idx = 0;
        Unit* unit;
        map<int,int> race2prof;
        string s;

        // assume that only dwarves have professions

        while( unit = getNext(&idx) ){
            s = unit->getName();
            if( s.find(", ") != string::npos ) race2prof[unit->getRace()]++;
        }

        if( race2prof.size() == 0 ){
            printf("[?] getDwarfRace: no units?\n");
            return -1;
        }

        if( race2prof.size() == 1 ){
            r = race2prof.begin()->first;
            return r;
        }

        // 2+ races in race2prof, select one with maximal number of units having profession
        int nmax=0;
        printf("[.] getDwarfRace:");
        for( map<int,int>::iterator it=race2prof.begin(); it != race2prof.end(); it++){
            if( it->second > nmax ){ r = it->first; nmax = it->second; }
            printf(" r%x:%d", it->first, it->second);
        }
        printf(" => r%x\n", r);

        return r;
    }

    //////////////////////////////////////////////////////////////////

    private:
    static const int        RACE_OFFSET =  0x44;
    static const int     COORD_X_OFFSET =  0x48; // word
    static const int     COORD_Y_OFFSET =  0x4a; // word
    static const int     COORD_Z_OFFSET =  0x4c; // word
    static const int       FLAGS_OFFSET =  0x8c;
    static const int         SEX_OFFSET =  0xa4; // word
    static const int          ID_OFFSET =  0xa8;
    //static const int WEAR_VECTOR_OFFSET = 0x22c; // 34.07: 0x22c, 34.10: 0x230, 31.24: 0x204
    //static const int PHYS_ATTRS_OFFSET = 0x338; // array of 8 entries of PHYS_ATTR_SIZE each
    //static const int       SOUL_OFFSET = 0x604; // ptr, also there's a vector of one same soul ptr at +0x5f8
    //static const int  HAPPINESS_OFFSET = 0x6a4; // dword

    static const int PHYS_ATTR_SIZE = 0x1c; // size of one physical attribute record
};
