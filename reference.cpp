#include "common.h"
#include "building.cpp"

class Reference : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    static const int REF_ARTIFACT                   = 0x00;
    static const int REF_IS_ARTIFACT                = 0x01;
    static const int REF_IS_NEMESIS                 = 0x03;
    static const int REF_ITEM_TYPE                  = 0x05;
    static const int REF_COINBATCH                  = 0x06;
    static const int REF_MAPSQUARE                  = 0x07;
    static const int REF_ENTITY_ART_IMAGE           = 0x08;
    static const int REF_CONTAINS_UNIT              = 0x09;
    static const int REF_CONTAINS_ITEM              = 0x0a;
    static const int REF_CONTAINED_IN_ITEM          = 0x0b;
    static const int REF_UNIT_MILKEE                = 0x0e;
    static const int REF_UNIT_TRAINEE               = 0x0f;
    static const int REF_UNIT_ITEMOWNER             = 0x10;
    static const int REF_UNIT_TRADEBRINGER          = 0x11;
    static const int REF_UNIT_HOLDER                = 0x12;
    static const int REF_UNIT_WORKER                = 0x13;
    static const int REF_UNIT_CAGEE                 = 0x14;
    static const int REF_UNIT_BEATEE                = 0x15;
    static const int REF_UNIT_FOODRECEIVER          = 0x16;
    static const int REF_UNIT_KIDNAPEE              = 0x17;
    static const int REF_UNIT_PATIENT               = 0x18;
    static const int REF_UNIT_INFANT                = 0x19;
    static const int REF_UNIT_SLAUGHTEREE           = 0x1a;
    static const int REF_UNIT_SHEAREE               = 0x1b;
    static const int REF_UNIT_SUCKEE                = 0x1c;
    static const int REF_UNIT_REPORTEE              = 0x1d;
    static const int REF_BUILDING_CIVZONE_ASSIGNED  = 0x1f;
    static const int REF_BUILDING_TRIGGER           = 0x20;
    static const int REF_BUILDING_TRIGGERTARGET     = 0x21;
    static const int REF_BUILDING_CHAIN             = 0x22;
    static const int REF_BUILDING_CAGED             = 0x23;
    static const int REF_BUILDING_HOLDER            = 0x24;
    static const int REF_BUILDING_WELL_TAG          = 0x25;
    static const int REF_BUILDING_USE_TARGET_1      = 0x26;
    static const int REF_BUILDING_USE_TARGET_2      = 0x27;
    static const int REF_BUILDING_DESTINATION       = 0x28;
    static const int REF_BUILDING_NEST_BOX          = 0x29;
    static const int REF_ENTITY                     = 0x2a;
    static const int REF_ENTITY_STOLEN              = 0x2b;
    static const int REF_ENTITY_OFFERED             = 0x2c;
    static const int REF_ENTITY_ITEMOWNER           = 0x2d;
    static const int REF_LOCATION                   = 0x2e;
    static const int REF_INTERACTION                = 0x2f;
    static const int REF_ABSTRACT_BUILDING          = 0x30;
    static const int REF_HISTORICAL_EVENT           = 0x31;
    static const int REF_SPHERE                     = 0x32;
    static const int REF_SITE                       = 0x33;
    static const int REF_SUBREGION                  = 0x34;
    static const int REF_FEATURE_LAYER              = 0x35;
    static const int REF_HISTORICAL_FIGURE          = 0x36;


    int getType(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return -1;
        return ((func_t_p)(((void**)pvtbl)[VTBL_FUNC_REF_TYPE_ID]))(this);
    }

    Item* getItem(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return NULL;
        // XXX will not work on 64bit
        return (Item*)((func_t_p)(((void**)pvtbl)[VTBL_FUNC_GET_ITEM]))(this);
    }

    Unit* getUnit(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return NULL;
        // XXX will not work on 64bit
        return (Unit*)((func_t_p)(((void**)pvtbl)[VTBL_FUNC_GET_UNIT]))(this);
    }

    Building* getBuilding(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return NULL;
        // XXX will not work on 64bit
        return (Building*)((func_t_p)(((void**)pvtbl)[VTBL_FUNC_GET_BUILDING]))(this);
    }

    const string getDescription(){
        static string s;
        if((func_t_pp)((void**)pvtbl)[VTBL_FUNC_DESCRIBE]){
            ((func_t_pp)((void**)pvtbl)[VTBL_FUNC_DESCRIBE])(this, &s);
        }
        return cp437_to_utf8(s);
    }

    const char* getTypeString(){
        switch(getType()){
            case REF_ARTIFACT                  : return "artifact";
            case REF_IS_ARTIFACT               : return "is artifact";
            case REF_IS_NEMESIS                : return "is nemesis";
            case REF_ITEM_TYPE                 : return "item type";
            case REF_COINBATCH                 : return "coinbatch";
            case REF_MAPSQUARE                 : return "mapsquare";
            case REF_ENTITY_ART_IMAGE          : return "entity art image";
            case REF_CONTAINS_UNIT             : return "contains unit";
            case REF_CONTAINS_ITEM             : return "contains item";
            case REF_CONTAINED_IN_ITEM         : return "contained in item";
            case REF_UNIT_MILKEE               : return "unit milkee";
            case REF_UNIT_TRAINEE              : return "unit trainee";
            case REF_UNIT_ITEMOWNER            : return "unit itemowner";
            case REF_UNIT_TRADEBRINGER         : return "unit tradebringer";
            case REF_UNIT_HOLDER               : return "unit holder";
            case REF_UNIT_WORKER               : return "unit worker";
            case REF_UNIT_CAGEE                : return "unit cagee";
            case REF_UNIT_BEATEE               : return "unit beatee";
            case REF_UNIT_FOODRECEIVER         : return "unit foodreceiver";
            case REF_UNIT_KIDNAPEE             : return "unit kidnapee";
            case REF_UNIT_PATIENT              : return "unit patient";
            case REF_UNIT_INFANT               : return "unit infant";
            case REF_UNIT_SLAUGHTEREE          : return "unit slaughteree";
            case REF_UNIT_SHEAREE              : return "unit shearee";
            case REF_UNIT_SUCKEE               : return "unit suckee";
            case REF_UNIT_REPORTEE             : return "unit reportee";
            case REF_BUILDING_CIVZONE_ASSIGNED : return "building civzone assigned";
            case REF_BUILDING_TRIGGER          : return "building trigger";
            case REF_BUILDING_TRIGGERTARGET    : return "building triggertarget";
            case REF_BUILDING_CHAIN            : return "building chain";
            case REF_BUILDING_CAGED            : return "building caged";
            case REF_BUILDING_HOLDER           : return "building holder";
            case REF_BUILDING_WELL_TAG         : return "building well tag";
            case REF_BUILDING_USE_TARGET_1     : return "building use target 1";
            case REF_BUILDING_USE_TARGET_2     : return "building use target 2";
            case REF_BUILDING_DESTINATION      : return "building destination";
            case REF_BUILDING_NEST_BOX         : return "building nest box";
            case REF_ENTITY                    : return "entity";
            case REF_ENTITY_STOLEN             : return "entity stolen";
            case REF_ENTITY_OFFERED            : return "entity offered";
            case REF_ENTITY_ITEMOWNER          : return "entity itemowner";
            case REF_LOCATION                  : return "location";
            case REF_INTERACTION               : return "interaction";
            case REF_ABSTRACT_BUILDING         : return "abstract building";
            case REF_HISTORICAL_EVENT          : return "historical event";
            case REF_SPHERE                    : return "sphere";
            case REF_SITE                      : return "site";
            case REF_SUBREGION                 : return "subregion";
            case REF_FEATURE_LAYER             : return "feature layer";
            case REF_HISTORICAL_FIGURE         : return "historical figure";
        }
        return "unknown";
    }

    //////////////////////////////////////////////////////////////////

    private:

    // index of functions in vtable
    static const int VTBL_FUNC_REF_TYPE_ID  =  2; // reference type
    static const int VTBL_FUNC_GET_ITEM     =  3;
    static const int VTBL_FUNC_GET_UNIT     =  4;
    static const int VTBL_FUNC_GET_BUILDING =  6;
    static const int VTBL_FUNC_DESCRIBE     = 18;
};

typedef vector<Reference*> RefsVector;
