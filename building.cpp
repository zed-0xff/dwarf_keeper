#include "common.h"

class Building : public MemClass {
    public:
    void *pvtbl;    // pointer to vtable

    string getName(){
        string s;
        ((func_t_pp)((void**)pvtbl)[VTBL_FUNC_GET_NAME])(this, &s);
        return s;
    }

    //////////////////////////////////////////////////////////////////

    private:

    // index of functions in vtable
    static const int VTBL_FUNC_GET_NAME = 0x30;
};
