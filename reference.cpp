#include "common.h"

class Reference : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    static const int REF_IS_ARTIFACT       =    1;
    static const int REF_IS_NEMESIS        =    3; // ???
    static const int REF_CONTAINS_UNIT     =    9;
    static const int REF_CONTAINS_ITEM     = 0x0a;
    static const int REF_UNIT_MILKEE       = 0x0e;
    static const int REF_UNIT_TRAINEE      = 0x0f;
    static const int REF_UNIT_ITEMOWNER    = 0x10;
    static const int REF_UNIT_TRADEBRINGER = 0x11;
    static const int REF_UNIT_CAGEE        = 0x14;
    static const int REF_UNIT_BEATEE       = 0x15;
    static const int REF_UNIT_FOODRECEIVER = 0x16;
    static const int REF_UNIT_KIDNAPEE     = 0x17;
    static const int REF_UNIT_PATIENT      = 0x18;
    static const int REF_UNIT_SHEAREE      = 0x1b;
    static const int REF_UNIT_SUCKEE       = 0x1c;
    static const int REF_UNIT_REPORTEE     = 0x1d;

    int getType(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return -1;
        return ((no_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_REF_TYPE_ID]))();
    }

    Item* getItem(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return NULL;
        // XXX will not work on 64bit
        return (Item*)((no_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_GET_ITEM]))();
    }

    Unit* getUnit(){
        if(pvtbl == NULL || ((int)pvtbl) == -1) return NULL;
        // XXX will not work on 64bit
        return (Unit*)((no_arg_func_t)(((void**)pvtbl)[VTBL_FUNC_GET_UNIT]))();
    }

    //////////////////////////////////////////////////////////////////

    private:

    // index of functions in vtable
    static const int VTBL_FUNC_REF_TYPE_ID = 2; // reference type
    static const int VTBL_FUNC_GET_ITEM    = 3;
    static const int VTBL_FUNC_GET_UNIT    = 4;
};

typedef vector<Reference*> RefsVector;
