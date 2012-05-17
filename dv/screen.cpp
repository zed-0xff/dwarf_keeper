#include <stdint.h>
#include <vector>

using namespace std;

#define DEFAULT_TILE_SIZE      18

class Screen {
    vector <uint32_t> data;
    vector <uint32_t> prev_data;

    public:
    int width, height;
    int tile_width, tile_height;

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

    void setData(uint32_t*src, int count = -1){
        if( count == -1 ) count = width*height;
        data.assign(src, src+count);
    }

    uint32_t at(int x, int y){
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
