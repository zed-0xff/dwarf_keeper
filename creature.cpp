#include "common.h"
#include "soul.cpp"

static const int RACE_CAT   = 0x09e;
static const int RACE_DUCK  = 0x0a8;
static const int RACE_DWARF = 0x1d1;

class Wearing{
    public:
    Item*item;
    int body_part;
};

class Creature;

typedef vector<Wearing*> WearingVector;
typedef vector<Creature*> CreaturesVector;

class Creature : public MemClass {
    public:

    static const int RECORD_SIZE = 0xa00;

    string getName(){
        string s;
        getCreatureFullName(this, &s, 0);
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

    void getCoords(int*px, int*py, int*pz){
        ((func_t_pppp)(CREATURE_COORDS_FUNC))(this, px, py, pz);
    }

    //////////////////////////////////////////////////////////////////

    static Creature* find(int id){
        CreaturesVector* v = (CreaturesVector*)CREATURES_VECTOR;
        for(int i=0; i<v->size(); i++){
            if(v->at(i)->getId() == id){
                return v->at(i);
            }
        }
        return NULL;
    }

    static Creature* getNext(int*idx, int race_filter = -1){
        CreaturesVector* v = (CreaturesVector*)CREATURES_VECTOR;
        Creature *pc;

        while((*idx) < v->size()){
            pc = v->at((*idx)++);

            // apply race_filter
            if( race_filter != -1 && pc->getRace() != race_filter ) 
                continue;

            // skip dead creatures
            if((pc->getFlags() & FLAG_DEAD) != 0) 
                continue;

            return pc;
        }

        return NULL;
    }
    //////////////////////////////////////////////////////////////////

    private:
    static const int      RACE_OFFSET =  0x44;
    static const int     FLAGS_OFFSET =  0x8c;
    static const int       SEX_OFFSET =  0xa4; // word
    static const int        ID_OFFSET =  0xa8;
    static const int      WEAR_OFFSET = 0x22c;
    static const int      SOUL_OFFSET = 0x604; // ptr, also there's a vector of one same soul ptr at +0x5f8
    static const int HAPPINESS_OFFSET = 0x6a4; // dword

};
