#include "common.h"

class MemClass {
    public:

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
