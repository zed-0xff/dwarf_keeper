#include "common.h"
#include "soul.cpp"

static const int RACE_CAT   = 0x09e;
static const int RACE_DUCK  = 0x0a8;
static const int RACE_DWARF = 0x1d1;

struct Wearing {
    Item*item;
    int body_part;
};

struct Coords {
    int16_t x,y,z;
};

static const char* PHYS_ATTR_NAMES[] = {"Strength", "Agility", "Toughness", "Endurance", "Recuperation", "Disease Resistance", "#7", "#8"};
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
        string s;
        getUnitFullName(this, &s, 0);
        return s;
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
        return (WearingVector*)((char*)this + WEAR_OFFSET);
    }

    int getHappiness(){
        return i(HAPPINESS_OFFSET);
    }

    string getThoughts(){
        string s;

        ((func_t_2_pvoids)(DWARF_THOUGHTS_FUNC))(this, &s);
        return s;
    }

    // 0 = FEMALE, 1 = MALE
    int getSex(){ return w(SEX_OFFSET); }

    Soul* getSoul(){ return (Soul*)(dw(SOUL_OFFSET)); } // XXX will not work on 64bit

    Coords getCoords(){
        Coords c = {0,0,0};
        ((func_t_pppp)(UNIT_COORDS_FUNC))(this, &c.x, &c.y, &c.z);
        return c;
    }

    PhysAttrsVector getPhysAttrs(){
        vector <PhysAttr> r;
        r.reserve(PHYS_ATTR_COUNT);
        for(int i=0; i<PHYS_ATTR_COUNT; i++){
            PhysAttr pa;
            pa.value = dw( PHYS_ATTRS_OFFSET + PHYS_ATTR_SIZE*i );
            pa.sub   = dw( PHYS_ATTRS_OFFSET + PHYS_ATTR_SIZE*i + 0x10 );
            pa.name  = PHYS_ATTR_NAMES[i];
            r.push_back(pa);
        }
        return r;
    }

    //////////////////////////////////////////////////////////////////

    static Unit* find(int id){
        UnitsVector* v = (UnitsVector*)UNITS_VECTOR;
        for(int i=0; i<v->size(); i++){
            if(v->at(i)->getId() == id){
                return v->at(i);
            }
        }
        return NULL;
    }

    static Unit* getNext(int*idx, int race_filter = -1){
        UnitsVector* v = (UnitsVector*)UNITS_VECTOR;
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
    //////////////////////////////////////////////////////////////////

    private:
    static const int       RACE_OFFSET =  0x44;
    static const int    COORD_X_OFFSET =  0x48; // word
    static const int    COORD_Y_OFFSET =  0x4a; // word
    static const int    COORD_Z_OFFSET =  0x4c; // word
    static const int      FLAGS_OFFSET =  0x8c;
    static const int        SEX_OFFSET =  0xa4; // word
    static const int         ID_OFFSET =  0xa8;
    static const int       WEAR_OFFSET = 0x22c;
    static const int PHYS_ATTRS_OFFSET = 0x338; // array of 8 entries of PHYS_ATTR_SIZE each
    static const int       SOUL_OFFSET = 0x604; // ptr, also there's a vector of one same soul ptr at +0x5f8
    static const int  HAPPINESS_OFFSET = 0x6a4; // dword

    static const int PHYS_ATTR_SIZE = 0x1c; // size of one physical attribute record
};
