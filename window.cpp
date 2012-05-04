#include "mem_class.cpp"

class Window : MemClass {
    public:
    uint32_t some_id;
    uint32_t xz0[2];

    uint32_t *vbuf, *vbuf_end;

    uint32_t xz1[5];
    uint32_t max_x;
    uint32_t xz2;
    uint32_t max_y;

    //////////////////////////////////////////////////////////////////

    static Window* root(){
        return (Window*)ROOT_WINDOW;
    }
};
