#include "common.h"

class Building;
typedef vector<Building*> BuildingsVector;

class Building : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    static const int RECORD_SIZE = 0xf0;

    int getId(){
        return dw(ID_OFFSET);
    }

    string getName(){
        string s;
        if( GAME.bld_vtbl_getname_offset ){
            ((func_t_pp)((void**)pvtbl)[ GAME.bld_vtbl_getname_offset/4 ])(this, &s);
        }

        if( s.empty() ){
            char buf[0x100];
            sprintf(buf, "#%d", getId());
            return buf;
        } else if( s == "Zone" ){
            char buf[0x100];
            sprintf(buf, "Activity Zone #%d", dw(ZONE_NUMBER_OFFSET));
            return buf;
        }
        return cp437_to_utf8(s);
    }

    int getValue(){
        if( GAME.bld_vtbl_getname_offset ){
            return ((func_t_p)((void**)pvtbl)[ GAME.bld_vtbl_getvalue_offset/4 ])(this);
        } else {
            return -1;
        }
    }

    static const int FLAG_FORBIDDEN    =    1; // not passable door
    static const int FLAG_INTERNAL     =    2;
    static const int FLAG_PET_PASSABLE = 0x40;

    uint32_t getFlags(){
        return dw(FLAGS_OFFSET);
    }

    string getFlagsString(){
        string s;
        uint32_t f = getFlags();
        FLAG_TO_STRING(FLAG_FORBIDDEN);
        FLAG_TO_STRING(FLAG_INTERNAL);
        FLAG_TO_STRING(FLAG_PET_PASSABLE);
        return s;
    }

    static const int FLAG_ZONE_WATER_SOURCE =     1;
    static const int FLAG_ZONE_FISHING      =  0x10;
    static const int FLAG_ZONE_GARBAGE_DUMP =     2;
    static const int FLAG_ZONE_PEN_PASTURE  = 0x100;
    static const int FLAG_ZONE_PIT_POND     =  0x20;
    static const int FLAG_ZONE_SAND         =     4;
    static const int FLAG_ZONE_CLAY         = 0x200;
    static const int FLAG_ZONE_MEETING_AREA =  0x40;
    static const int FLAG_ZONE_HOSPITAL     =  0x80;
    static const int FLAG_ZONE_ANIMAL_TRAIN = 0x400;
    static const int FLAG_ZONE_ACTIVE       =     8;

    uint32_t zoneFlags(){
        return dw(ZONE_FLAGS_OFFSET);
    }

    string zoneFlagsString(){
        string s;
        uint32_t f = zoneFlags();
        FLAG_TO_STRING(FLAG_ZONE_WATER_SOURCE);
        FLAG_TO_STRING(FLAG_ZONE_FISHING);      
        FLAG_TO_STRING(FLAG_ZONE_GARBAGE_DUMP); 
        FLAG_TO_STRING(FLAG_ZONE_PEN_PASTURE);  
        FLAG_TO_STRING(FLAG_ZONE_PIT_POND);     
        FLAG_TO_STRING(FLAG_ZONE_SAND);         
        FLAG_TO_STRING(FLAG_ZONE_CLAY);         
        FLAG_TO_STRING(FLAG_ZONE_MEETING_AREA); 
        FLAG_TO_STRING(FLAG_ZONE_HOSPITAL);     
        FLAG_TO_STRING(FLAG_ZONE_ANIMAL_TRAIN); 
        FLAG_TO_STRING(FLAG_ZONE_ACTIVE);       
        return s;
    }

    WearingVector* getItems(){
        // XXX not all buildings have items!
        // there may be random data at vector offset!
        return (WearingVector*)checked_vector(ITEMS_VECTOR_OFFSET);
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
        return (BuildingsVector*)checked_global_vector(GAME.buildings_vector);
    }

    static Building* find(int id){
        if( BuildingsVector*v = getVector() ){
            for(int i=0; i<v->size(); i++){
                if( v->at(i)->getId() == id ) return v->at(i);
            }
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
    static const int FLAGS_OFFSET        = 0xc8;

    static const int ZONE_FLAGS_OFFSET   = 0xd0; // only for zones
    static const int ZONE_NUMBER_OFFSET  = 0xe4; // only for zones
};

