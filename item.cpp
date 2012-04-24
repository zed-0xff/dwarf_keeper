#include "common.h"

typedef vector<Item*> ItemsVector;

class Item : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    static const int RECORD_SIZE = 0xb0;

    string getName(){
        string s;
        getItemName(this, &s, 0, -1);
        return s;
    }

    string getBaseName(int mode){
        // mode 0: dimple cups [3]
        // mode 1: dimple cup
        // mode 2: dimple cups
        string s;
        getItemBaseName(this, &s, mode);
        return s;
    }

    int getValue(){
        return getItemValue(this, 0, 0);
    }

    int getId(){
        return *(int*)((char*)this+ID_OFFSET);
    }

    int getTypeId(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return -1;
        return ((no_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_TYPE_ID]))();
    }

    int getSubTypeId(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return -1;
        return ((pvoid_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_SUBTYPE_ID]))(this);
    }

    uint32_t inline getFullId(){
        return (getTypeId()<<16) + getSubTypeId();
    }

    int getRaceId(){
        return *(int16_t*)((char*)this+RACE_ID_OFFSET);
    }

    // http://dwarffortresswiki.org/index.php/40d:Item_designations#Wear
 
    static const int WEAR_OK = 0;
    static const int WEAR_x  = 1;
    static const int WEAR_X  = 2;
    static const int WEAR_XX = 3;

    int getWear(){
        return *(int16_t*)((char*)this+WEAR_OFFSET);
    }

    static const int SIZE_OK    = 0;
    static const int SIZE_SMALL = 1;
    static const int SIZE_LARGE = 2;

    int getSize(){
        // int cmp_item_size(int itemType, int itemSubType, int race_id_1, int race_id_2)
        return ((func_t_4_ints)(CMP_ITEMSIZE_FUNC))(
                getTypeId(),
                getSubTypeId(),
                getRaceId(),
                RACE_DWARF
                );
    }

    //////////////////////////////////////////////////////////////////

    static ItemsVector* getVector(){
        return (ItemsVector*)ITEMS_VECTOR;
    }

    //////////////////////////////////////////////////////////////////

    private:
    // offset in instance data
    static const int ID_OFFSET         = 0x14;
    static const int WEAR_OFFSET       = 0x6c; // word
    static const int RACE_ID_OFFSET    = 0x84; // word

    // index of functions in vtable
    static const int VTBL_FUNC_TYPE_ID    = 0; // static - no args
    static const int VTBL_FUNC_SUBTYPE_ID = 1; // one arg - self
};
