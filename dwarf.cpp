#include "common.h"

class Wearing{
    public:
    Item*item;
    int body_part;
};

typedef vector<Wearing*> WearingVector;

class Dwarf{
    public:

    static const int  RACE_OFFSET = 0x44;
    static const int FLAGS_OFFSET = 0x8c;
    static const int    ID_OFFSET = 0xa8;
    static const int  WEAR_OFFSET = 0x22c;

    string getName(){
        string s;
        getCreatureFullName(this, &s, 0);
        return s;
    }

    int getId(){
        return *(int*)((char*)this+ID_OFFSET);
    }

    static const int FLAG_DEAD = 2;

    int getFlags(){
        return *(int*)((char*)this+FLAGS_OFFSET);
    }

    static const int RACE_CAT   = 0x09e;
    static const int RACE_DUCK  = 0x0a8;
    static const int RACE_DWARF = 0x1d1;

    int getRace(){
        return *(int*)((char*)this+RACE_OFFSET);
    }

    WearingVector* getWear(){
        return (WearingVector*)((char*)this + WEAR_OFFSET);
    }

    static Dwarf* getNext(int*idx){
        void **vector, **vend;
        Dwarf *pDwarf;

        vector = *(void***)CREATURES_VECTOR;
        vend   = *(void***)(CREATURES_VECTOR+4);
        
        while((vector+*idx) < vend){
            pDwarf = (Dwarf*)vector[(*idx)++];
            if(!pDwarf) break;

            if((pDwarf->getRace() == RACE_DWARF) && ((pDwarf->getFlags() & FLAG_DEAD) == 0)){
                return pDwarf;
            }
        }

        return NULL;
    }
};
