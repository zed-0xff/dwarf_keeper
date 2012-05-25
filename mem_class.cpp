#ifndef MEM_CLASS_H
#define MEM_CLASS_H

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

    static const int DEFAULT_MAX_DIFF = 16777216; // 0x1000000

    static void* checked_global_vector(void*offset, int max_diff=DEFAULT_MAX_DIFF){
        // in-memory storage of vector consists of 3 pointers:
        //  a) data start
        //  b) last used entry ptr
        //  c) last allocated entry ptr
        //
        // so, must be: a <= b <= c
        //
        void **p = (void**)offset;

        if( p[0] && p[1] && p[2] && (p[0] <= p[1]) && (p[1] <= p[2]) ){
            if( (max_diff == 0) || (((void**)p[2]-(void**)p[0]) <= max_diff) ){
                return p;
            }
        }
        return NULL;
    }

    void* checked_vector(int offset, int max_diff=DEFAULT_MAX_DIFF){
        return checked_global_vector((char*)this+offset, max_diff);
    }
};

#endif
