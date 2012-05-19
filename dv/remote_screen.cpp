#include <stdint.h>
#include <vector>

using namespace std;

#define DEFAULT_TILE_SIZE      18

class RemoteScreen {
    vector <uint16_t> data;

    public:
    int width, height;
    int tile_width, tile_height;
    uint32_t hash;

    RemoteScreen(){
        tile_width = tile_height = DEFAULT_TILE_SIZE;
        width = height = 0;
    }

//    void operator = (RemoteScreen&scr){
//        data = scr.data;
//        width = scr.width;
//        height = scr.height;
//        tile_width = scr.tile_width;
//        tile_height = scr.tile_height;
//        hash = scr.hash;
//    }

    int pixelWidth(){
        return tile_width * width;
    }

    int pixelHeight(){
        return tile_height * height;
    }

    void setData(uint16_t*src, int count = -1){
        if( !src ) return;
        if( count == -1 ) count = width*height;
        data.assign(src, src+count);
        hash = 0;
        for( int i=0; i<data.size(); i++){
            hash ^= data[i];
            hash = hash << 1 | hash >> 31;     // circular shift 1 bit left
        }
    }

    uint16_t at(int x, int y){
        try{
            return data.at(x*height+y);
        } catch(...){
            printf("[d] x=%d, y=%d\n", x, y);
            throw;
        }
    }

};
