#include <unordered_map>
#include <snappy.h>

struct tile_fullid {
    uint32_t c;
    uint32_t texpos;
    uint8_t ad, gr, cf, cb;
};

class CopiedScreen {

    struct Cmp {
        bool operator() ( const tile_fullid& t1, const tile_fullid& t2 ){
            return t1.texpos < t2.texpos;
        }
    };

    int w,h;

    vector<uint32_t> screen, screentexpos;
    vector<uint8_t>  screentexpos_ad, screentexpos_gr, screentexpos_cf, screentexpos_cb;

    pthread_mutex_t mutex;
    pthread_cond_t  cond_changed;

    vector<uint16_t> prepared_data;
    uint32_t         prepared_hash;

    unordered_map<uint32_t,uint16_t> tiles2nos;
    map<tile_fullid,uint16_t,Cmp> tfids2nos;
    uint16_t max_id;

    string compressed_data;

    public:

    CopiedScreen(){
        mutex = PTHREAD_MUTEX_INITIALIZER;
        cond_changed = PTHREAD_COND_INITIALIZER;
        max_id = 0;
        prepared_hash = 0;
    }

    // can cause weird results if not locked
    string* prepare_data(){
        prepared_hash = 0;

        prepared_data.resize(screen.size()+2);
        prepared_data[0] = w & 0xffff; // width
        prepared_data[1] = h & 0xffff; // height
        int n = screen.size();
        uint16_t t;

        for(int i=0; i<n; i++){
            t = tile2no(i);
            prepared_data[i+2] = t;
            prepared_hash ^= t;
            prepared_hash = prepared_hash << 1 | prepared_hash >> 31; // ROL 1
        }

        compressed_data.clear();
        snappy::Compress((const char*)prepared_data.data(), prepared_data.size()*2, &compressed_data);

        return &compressed_data;
    }

    tile_fullid tileno2tile(uint16_t tileno, bool*pfound = NULL){
        // iteration is slower than map lookup, but this function should be
        // called a relatively few number of times
        for( auto it = tiles2nos.begin(); it != tiles2nos.end(); ++it ){
            if( it->second == tileno ){
                if(pfound) *pfound = true;
                return (tile_fullid){ c: it->first, texpos: 0 };
            }
        }
        for( auto it = tfids2nos.begin(); it != tfids2nos.end(); ++it ){
            if( it->second == tileno ){
                if(pfound) *pfound = true;
                return it->first;
            }
        }
        if(pfound) *pfound = false;
        return (tile_fullid){ 0 };
    }

    uint16_t tile2no(int idx){
        if( screentexpos[idx] ){
            tile_fullid tfid;
            tfid.c = screen[idx];
            if( tfid.texpos = screentexpos[idx] ){
                tfid.ad = screentexpos_ad[idx];
                tfid.gr = screentexpos_gr[idx];
                tfid.cf = screentexpos_cf[idx];
                tfid.cb = screentexpos_cb[idx];
            } else {
                tfid.ad = tfid.gr = tfid.cf = tfid.cb = 0;
            }

            uint16_t r = tfids2nos[tfid];
            if( r ){
                return r;
            } else {
                max_id++;
                tfids2nos[tfid] = max_id;
                return max_id;
            } 
        } else {
            uint32_t c = screen[idx];
            uint16_t r = tiles2nos[c];
            if( r ){
                return r;
            } else {
                max_id++;
                tiles2nos[c] = max_id;
                return max_id;
            } 
        }
    }

    void copy(){
        if( gps.screen && gps.dimx > 0 && gps.dimx < 1000 && gps.dimy > 0 && gps.dimy < 1000 ){
            lock();

            uint32_t h0 = raw_hash();

            w = gps.dimx; h = gps.dimy;
            int n = w*h;

            screen.resize(n);
            screentexpos.resize(n);
            screentexpos_ad.resize(n);
            screentexpos_gr.resize(n);
            screentexpos_cf.resize(n);
            screentexpos_cb.resize(n);

            memcpy(screen.data(), gps.screen, n*4);
            memcpy(screentexpos.data(), gps.screentexpos, n*4);
            memcpy(screentexpos_ad.data(), gps.screentexpos_addcolor, n);
            memcpy(screentexpos_gr.data(), gps.screentexpos_grayscale, n);
            memcpy(screentexpos_cf.data(), gps.screentexpos_cf, n);
            memcpy(screentexpos_cb.data(), gps.screentexpos_cbr, n);

            if( h0 != raw_hash() ){
                prepared_hash = 0;
                pthread_cond_broadcast(&cond_changed);
            }

            unlock();
        }
    }

    void lock(){
        pthread_mutex_lock(&mutex);
    }

    void unlock(){
        pthread_mutex_unlock(&mutex);
    }

    void wait(){
        struct timeval tv;
        struct timespec ts;
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + 1;
        ts.tv_nsec = 0;

        pthread_cond_timedwait(&cond_changed, &mutex, &ts);
    }

    // NO LOCKS INSIDE! must be locked before call
    uint32_t raw_hash(){
        uint32_t hash = 0;
        int n = screen.size();

        for( int i = 0; i<n; i++){
            hash ^= screen[i];
            hash = hash << 1 | hash >> 31;     // circular shift 1 bit left
        }

        return hash;
    }

    // true if screen changed since prev_hash value
    bool changed( uint32_t prev_hash ){
        return (!prev_hash || !prepared_hash || prev_hash != prepared_hash);
    }

    bool valid(){
        return screen.size() > 0;
    }
};
