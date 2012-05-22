#include "common.h"
#include "reference.cpp"
#include "item_type.cpp"

class Item : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    // 0xb0 for clothes
    // 0x90 for other items
    // +may be other sizes depending on item type
    static const int RECORD_SIZE = 0xb0; 

    string getName(){
        if( GAME.item_name_func ){
            string s;
            ((func_t_ppii)GAME.item_name_func)(this, &s, 0, -1);
            return cp437_to_utf8(s);
        } else {
            return "Error: getItemName is NULL";
        }
    }

    string getBaseName(int mode){
        // mode 0: dimple cups [3]
        // mode 1: dimple cup
        // mode 2: dimple cups

        if( GAME.item_base_name_func ){
            string s;
            ((func_t_ppi)GAME.item_base_name_func)(this, &s, mode);
            return cp437_to_utf8(s);
        } else {
            return "Error: getItemBaseName is NULL";
        }
    }

    // http://dwarffortresswiki.org/index.php/Stocks#Color_Code
    static const int FLAG_PRESERVED      =          4;
    static const int FLAG_BUILDING_PART  =       0x20;
    static const int FLAG_ROTTEN         =      0x100; // rotten or withered
    static const int FLAG_WEB            =      0x200; // set on spiders silk webs
    static const int FLAG_IMPORTED       =     0x4000; // "(birchen bucket)" - round brackets
    static const int FLAG_NOT_FORT_OWN   =     0x8000; // not belongs to fort
    static const int FLAG_HAS_OWNER      =    0x10000; // item is owned by some creature
    static const int FLAG_DUMP           =    0x20000; // item is marked for dumping, but not dumped yet
    static const int FLAG_ARTIFACT       =    0x40000; // artifact, both local or foreign
    static const int FLAG_FORBID         =    0x80000;
    static const int FLAG_ON_FIRE        =   0x400000; // burning
    static const int FLAG_MELT           =   0x800000; // designated for melting
    static const int FLAG_HIDE           =  0x1000000;
    static const int FLAG_HOSPITAL       =  0x2000000; // hospital property (items are kept in hospital chests)
    static const int FLAG_LOCAL_ARTIFACT =  0x8000000; // artifact made by fort citizen
    //static const int FLAG_UNTANNED_HIDE = 0x80000000; // not sure

    string getFlagsString(){
        string s;
        uint32_t f = getFlags();
        FLAG_TO_STRING(FLAG_PRESERVED);
        FLAG_TO_STRING(FLAG_BUILDING_PART); 
        FLAG_TO_STRING(FLAG_ROTTEN);        
        FLAG_TO_STRING(FLAG_WEB);
        FLAG_TO_STRING(FLAG_IMPORTED);      
        FLAG_TO_STRING(FLAG_NOT_FORT_OWN);  
        FLAG_TO_STRING(FLAG_HAS_OWNER);     
        FLAG_TO_STRING(FLAG_DUMP);          
        FLAG_TO_STRING(FLAG_ARTIFACT);      
        FLAG_TO_STRING(FLAG_FORBID);        
        FLAG_TO_STRING(FLAG_ON_FIRE);        
        FLAG_TO_STRING(FLAG_MELT);          
        FLAG_TO_STRING(FLAG_HIDE);          
        FLAG_TO_STRING(FLAG_HOSPITAL);      
        FLAG_TO_STRING(FLAG_LOCAL_ARTIFACT);
        return s;
    }

    uint32_t getFlags(){
        return dw(FLAGS_OFFSET);
    }

    int getValue(){
        if( GAME.item_value_func ){
            return ((func_t_pii)GAME.item_value_func)(this, 0, 0);
        } else {
            return -1;
        }
    }

    // self value + sum of all values of contained items, if any
    int getPrice(){
        int price = getValue();
        if( RefsVector* rv = getRefs() ){
            for(int i=0; i<rv->size(); i++){
                if(rv->at(i)->getType() == Reference::REF_CONTAINS_ITEM){
                    price += rv->at(i)->getItem()->getValue();
                }
            }
        }
        return price;
    }

    Unit* getOwner(){
        if( RefsVector* rv = getRefs() ){
            for(int i=0; i<rv->size(); i++){
                if(rv->at(i)->getType() == Reference::REF_UNIT_ITEMOWNER){
                    return rv->at(i)->getUnit();
                }
            }
        }
        return NULL;
    }

    int getId(){
        return dw(ID_OFFSET);
    }

    int getTypeId(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return -1;
        return ((no_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_TYPE_ID]))();
    }

    int getSubTypeId(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return -1;
        return 0xffff & ((pvoid_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_SUBTYPE_ID]))(this);
    }

    uint32_t inline getFullId(){
        return (getTypeId()<<16) + getSubTypeId();
    }

    int getRaceId(){
        if( GAME.unit_refs_vector_offset ){
            return w(RACE_ID_OFFSET - 0x24 + GAME.unit_refs_vector_offset);
        } else {
            return w(RACE_ID_OFFSET);
        }
    }

    // http://dwarffortresswiki.org/index.php/40d:Item_designations#Wear
 
    static const int WEAR_OK = 0;
    static const int WEAR_x  = 1;
    static const int WEAR_X  = 2;
    static const int WEAR_XX = 3;

    int getWear(){
        if( GAME.unit_refs_vector_offset ){
            return w(WEAR_OFFSET - 0x24 + GAME.unit_refs_vector_offset);
        } else {
            return w(WEAR_OFFSET);
        }
    }

    static const int SIZE_OK    = 0;
    static const int SIZE_SMALL = 1;
    static const int SIZE_LARGE = 2;

    int getSize(){
        if(!GAME.cmp_item_size_func){
            return SIZE_OK;
        }

        // int cmp_item_size(int itemType, int itemSubType, int race_id_1, int race_id_2)
        return ((func_t_4_ints)(GAME.cmp_item_size_func))(
                getTypeId(),
                getSubTypeId(),
                getRaceId(),
                Unit::getDwarfRace()
                );
    }

    RefsVector* getRefs(){
        if( GAME.unit_refs_vector_offset ){
            return (RefsVector*)checked_vector(GAME.unit_refs_vector_offset);
        } else {
            return NULL;
        }
    }

    Coords getCoords(){
        Coords c = {0,0,0};
        c.x = i16(COORD_X_OFFSET);
        c.y = i16(COORD_Y_OFFSET);
        c.z = i16(COORD_Z_OFFSET);
        return c;
    }

    //////////////////////////////////////////////////////////////////

    static ItemsVector* getVector(){
        return (ItemsVector*)GAME.items_vector;
    }

    static Item* find(int id){
        if( ItemsVector*v = getVector() ){
            for(int i=0; i<v->size(); i++){
                if( v->at(i)->getId() == id ) return v->at(i);
            }
        }
        return NULL;
    }

    //////////////////////////////////////////////////////////////////

    private:
    // offset in instance data
    static const int COORD_X_OFFSET         = 0x04; // word
    static const int COORD_Y_OFFSET         = 0x06; // word
    static const int COORD_Z_OFFSET         = 0x08; // word
    static const int FLAGS_OFFSET           = 0x0c;
    static const int ID_OFFSET              = 0x14;
    //static const int REFS_VECTOR_OFFSET     = 0x24; // item references vector, 3x4 bytes
    //static const int AMOUNT_OFFSET          = 0x58; // 20 in "iron bolts [20]"
    static const int WEAR_OFFSET            = 0x6c; // word - item condition
    //static const int MATERIAL_ID_OFFSET     = 0x7c;
    //static const int MATERIAL_SUB_ID_OFFSET = 0x80;
    static const int RACE_ID_OFFSET         = 0x84; // word
    //static const int QUALITY_OFFSET         = 0x86; // word

    // index of functions in vtable
    static const int VTBL_FUNC_TYPE_ID    = 0; // static - no args
    static const int VTBL_FUNC_SUBTYPE_ID = 1; // one arg - self
};
