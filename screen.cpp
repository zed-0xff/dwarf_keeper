#include "common.h"

class Screen {
    public:

    static void moveTo(int x, int y, int z, int mode=2){
        *((int*)SCR_TARGET_CENTER_X) = x;
        *((int*)SCR_TARGET_CENTER_Y) = y;
        *((int*)SCR_TARGET_CENTER_Z) = z;

        // possible argument values are 0,1,2
        //  2  - move screen center exactly on point
        // 0,1 - do a minimal possible moves, leaving cursor not exactly in screen center
        ((func_t_i)(SET_SCREEN_CENTER_FUNC))(mode);
    }
};
