#include "common.h"

#define FLAG_TO_STRING(FLAG) if( f & FLAG ) s+= ((#FLAG " ")+5)

class MemClass {
    public:

    int16_t i16(int offset){
        return *(int16_t*)((char*)this+offset);
    }
    uint16_t w(int offset){
        return *(uint16_t*)((char*)this+offset);
    }
    uint32_t dw(int offset){
        return *(uint32_t*)((char*)this+offset);
    }
    int32_t i(int offset){
        return *(int32_t*)((char*)this+offset);
    }
};
