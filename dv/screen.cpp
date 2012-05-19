#include <stdint.h>
#include <vector>

using namespace std;

#define DEFAULT_TILE_SIZE      18

class Screen {
    vector <uint16_t> data;
    vector <uint16_t> prev_data;

    public:
    int width, height;
    int tile_width, tile_height;
    uint32_t hash;

    Screen(){
        tile_width = tile_height = DEFAULT_TILE_SIZE;
        width = height = 0;
    }

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

    void save(){
        prev_data = data;
    }

    bool changed(){
        return(prev_data != data);
    }
};
