#include "common.h"

#ifndef SCREEN_H
#define SCREEN_H

typedef vector<uint8_t> ChecksVector;
typedef vector<int32_t> AmountsVector;

struct TradeSideInfo {
    ItemsVector   *items;
    ChecksVector  *checks;
    AmountsVector *amounts;

    void toggle_by_index(int idx, bool state){
        checks->at(idx) = state ? 1 : 0;
    }

    Item* toggle_item(int id, bool state){
        for( int i=0; i < items->size(); i++){
            Item *item = items->at(i);
            if(item->getId() == id){
                if(state){
                    // toggle ON
                    checks->at(i) = 1;
                } else {
                    // toggle OFF
                    checks->at(i) = 0;
                }
                return item;
            }
        }
        return NULL;
    }
};

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

    static const int TRADE_SIDE_LEFT  = 0;
    static const int TRADE_SIDE_RIGHT = 1;

    TradeSideInfo getTradeSideInfo( int side ){
        TradeSideInfo tsi;
        if( side == TRADE_SIDE_LEFT){
            tsi.items  = (ItemsVector*)((char*)this  + LEFT_TRADE_ITEMS_VECTOR_OFFSET);
            tsi.checks = (ChecksVector*)((char*)this + LEFT_TRADE_CHECKS_VECTOR_OFFSET);
            tsi.amounts= (AmountsVector*)((char*)this+ LEFT_TRADE_AMOUNTS_VECTOR_OFFSET);
        } else {
            tsi.items  = (ItemsVector*)((char*)this  + RIGHT_TRADE_ITEMS_VECTOR_OFFSET);
            tsi.checks = (ChecksVector*)((char*)this + RIGHT_TRADE_CHECKS_VECTOR_OFFSET);
            tsi.amounts= (AmountsVector*)((char*)this+ RIGHT_TRADE_AMOUNTS_VECTOR_OFFSET);
        }
        return tsi;
    }

    //////////////////////////////////////////////////////////////////

    static void moveTo(Coords c, int mode=2){
        if( GAME.scr_target_center_px && GAME.scr_target_center_py && GAME.scr_target_center_pz){
            *((int*)GAME.scr_target_center_px) = c.x;
            *((int*)GAME.scr_target_center_py) = c.y;
            *((int*)GAME.scr_target_center_pz) = c.z;
        }

        // possible argument values are 0,1,2
        //  2  - move screen center exactly on point
        // 0,1 - do a minimal possible moves, leaving cursor not exactly in screen center
        if( GAME.set_screen_center_func ){
            ((func_t_i)(GAME.set_screen_center_func))(mode);
        }
    }

    // get current screen center
    static Coords getCenter(){
        Coords c = {-1, -1, -1};
        if( GAME.scr_target_center_px && GAME.scr_target_center_py && GAME.scr_target_center_pz){
            c.x = *((int*)GAME.scr_target_center_px);
            c.y = *((int*)GAME.scr_target_center_py);
            c.z = *((int*)GAME.scr_target_center_pz);
        }
        return c;
    }

    static Screen* root(){
        if( GAME.root_screen ){
            return (Screen*)GAME.root_screen;
        } else {
            return NULL;
        }
    }

//    static void enumerate(){
//        Screen *s = root();
//        while(s){
//            printf("[.] screen %10p: vtbl=%10p : %s \n", s, s->pvtbl, s->vtbl2type());
//            s = s->next();
//        }
//    }
//
    //////////////////////////////////////////////////////////////////

    private:
    // !!! only for Trade Screen !!!
    static const int LEFT_TRADE_ITEMS_VECTOR_OFFSET    = 0x130;
    static const int RIGHT_TRADE_ITEMS_VECTOR_OFFSET   = 0x13c;

    static const int LEFT_TRADE_CHECKS_VECTOR_OFFSET   = 0x148;
    static const int RIGHT_TRADE_CHECKS_VECTOR_OFFSET  = 0x154;

    static const int LEFT_TRADE_AMOUNTS_VECTOR_OFFSET  = 0x160;
    static const int RIGHT_TRADE_AMOUNTS_VECTOR_OFFSET = 0x16c;
};

#endif
