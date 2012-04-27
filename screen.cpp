#include "common.h"

class Screen {
    void *pvtbl;
    Screen *_next, *_prev; // double-linked list

    public:

    Screen* next(){ return _next; }
    Screen* prev(){ return _prev; }

    const char* vtbl2type(){
        void **p = (void**)pvtbl;
        if(!p) return NULL;
        p--;
        p=(void**)*p;
        if(!p) return NULL;
        p++;
        return (char*)*p;
    }

    ItemsVector* getLeftTradeVector(){
        return (ItemsVector*)((char*)this + LEFT_TRADE_VECTOR_OFFSET);
    }

    ItemsVector* getRightTradeVector(){
        return (ItemsVector*)((char*)this + RIGHT_TRADE_VECTOR_OFFSET);
    }

    //////////////////////////////////////////////////////////////////

    static void moveTo(int x, int y, int z, int mode=2){
        *((int*)SCR_TARGET_CENTER_X) = x;
        *((int*)SCR_TARGET_CENTER_Y) = y;
        *((int*)SCR_TARGET_CENTER_Z) = z;

        // possible argument values are 0,1,2
        //  2  - move screen center exactly on point
        // 0,1 - do a minimal possible moves, leaving cursor not exactly in screen center
        ((func_t_i)(SET_SCREEN_CENTER_FUNC))(mode);
    }

    static Screen* root(){
        return (Screen*)ROOT_SCREEN;
    }

    static void enumerate(){
        Screen *s = root();
        while(s){
            printf("[.] screen %10p: vtbl=%10p : %s \n", s, s->pvtbl, s->vtbl2type());
            s = s->next();
        }
    }

    //////////////////////////////////////////////////////////////////

    private:
    // !!! only for Trade Screen !!!
    static const int LEFT_TRADE_VECTOR_OFFSET  = 0x130;
    static const int RIGHT_TRADE_VECTOR_OFFSET = 0x13c;
};
