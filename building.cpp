#include "common.h"

class Building;
typedef vector<Building*> BuildingsVector;

#define FLAG_TO_STRING(FLAG) if( f & FLAG ) s+= ((#FLAG " ")+5)

class Building : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    static const int RECORD_SIZE = 0xf0;

    int getId(){
        return dw(ID_OFFSET);
    }

    string getName(){
        string s;
        ((func_t_pp)((void**)pvtbl)[VTBL_FUNC_GET_NAME])(this, &s);
        if( s == "Zone" ){
            char buf[0x100];
            sprintf(buf, "Activity Zone #%d", dw(ZONE_NUMBER_OFFSET));
            s = buf;
        }
        return s;
    }

    static const int FLAG_WATER_SOURCE =     1;
    static const int FLAG_FISHING      =  0x10;
    static const int FLAG_GARBAGE_DUMP =     2;
    static const int FLAG_PEN_PASTURE  = 0x100;
    static const int FLAG_PIT_POND     =  0x20;
    static const int FLAG_SAND         =     4;
    static const int FLAG_CLAY         = 0x200;
    static const int FLAG_MEETING_AREA =  0x40;
    static const int FLAG_HOSPITAL     =  0x80;
    static const int FLAG_ANIMAL_TRAIN = 0x400;
    static const int FLAG_ACTIVE       =     8;

    uint32_t zoneFlags(){
        return dw(ZONE_FLAGS_OFFSET);
    }

    string zoneFlagsString(){
        string s;
        uint32_t f = zoneFlags();
        FLAG_TO_STRING(FLAG_WATER_SOURCE);
        FLAG_TO_STRING(FLAG_FISHING);      
        FLAG_TO_STRING(FLAG_GARBAGE_DUMP); 
        FLAG_TO_STRING(FLAG_PEN_PASTURE);  
        FLAG_TO_STRING(FLAG_PIT_POND);     
        FLAG_TO_STRING(FLAG_SAND);         
        FLAG_TO_STRING(FLAG_CLAY);         
        FLAG_TO_STRING(FLAG_MEETING_AREA); 
        FLAG_TO_STRING(FLAG_HOSPITAL);     
        FLAG_TO_STRING(FLAG_ANIMAL_TRAIN); 
        FLAG_TO_STRING(FLAG_ACTIVE);       
        return s;
    }

    WearingVector* getItems(){
        if( getFlag1B() && getFlag25() ){
            return (WearingVector*)((char*)this + ITEMS_VECTOR_OFFSET);
        } else {
            return NULL;
        }
    }

    int getFlag1B(){
        return ((no_arg_func_t)((void**)pvtbl)[VTBL_FUNC_FLAG_1B])();
    }

    int getFlag25(){
        return ((no_arg_func_t)((void**)pvtbl)[VTBL_FUNC_FLAG_25])();
    }

    Coords getCoords(){
        Coords c;
        c.x = dw(CENTER_X_OFFSET);
        c.y = dw(CENTER_Y_OFFSET);
        c.z = dw(CENTER_Z_OFFSET);
        return c;
    }

    //////////////////////////////////////////////////////////////////
    
    static BuildingsVector* getVector(){
        return (BuildingsVector*)BUILDINGS_VECTOR;
    }

    static Building* find(int id){
        BuildingsVector*v = getVector();
        for(int i=0; i<v->size(); i++){
            if( v->at(i)->getId() == id ) return v->at(i);
        }
        return NULL;
    }

    //////////////////////////////////////////////////////////////////

    private:
    // offset in instance data
    
    static const int ID_OFFSET           = 0x48;
    static const int ITEMS_VECTOR_OFFSET = 0xb8; // vector of ptrs to 4xdw records, first dword of each is a ptr to Item
    static const int CENTER_X_OFFSET     = 0x0c; // dword!
    static const int CENTER_Y_OFFSET     = 0x18; // dword!
    static const int CENTER_Z_OFFSET     = 0x1c; // dword!

    static const int ZONE_FLAGS_OFFSET   = 0xd0; // only for zones
    static const int ZONE_NUMBER_OFFSET  = 0xe4; // only for zones


    // index of functions in vtable
    static const int VTBL_FUNC_FLAG_1B   = 0x1b; // if true then item has vector at ITEMS_VECTOR_OFFSET
    static const int VTBL_FUNC_FLAG_25   = 0x25; // if true then item has vector at ITEMS_VECTOR_OFFSET
    static const int VTBL_FUNC_GET_NAME  = 0x30;
};

