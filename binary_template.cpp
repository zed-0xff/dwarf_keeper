#include "game.h"

class BinaryTemplate {

    enum tpl_part_type {
        PART_DATA,
        PART_SKIP,
        PART_ARG
    };

    struct tpl_part {
        //char is_data; // argument otherwise
        tpl_part_type type;
        string data;
        int size;
    };

    vector <tpl_part> vtpl;
    vector <uint32_t> results;
    int align;

    public:

    int debug;

    BinaryTemplate( const char*tpl, int align_=4 ){ // DWORD align by default
        string s;
        char c, *p1;
        const char*p=tpl;
        tpl_part tp = {PART_DATA, "", 0};
        debug = 0;

        align = align_;

        while( p && *p){
//            printf("[d] %s\n", p);
            if(*p == '?'){
                if( tp.type != PART_SKIP && tp.size > 0 ){
                    vtpl.push_back(tp);
                    tp.data.clear();
                    tp.size = 0;
                }
                tp.type = PART_SKIP;

                p++; // skip first question mark
                if(*p != '?'){
                    printf("[!] invalid template, line %d: '%s'\n", __LINE__, p);
                    return;
                }
                p++; // skip second question mark
                tp.size++;

                if(!*p) break; // EOTPL
                if(*p != ' '){
                    printf("[!] invalid template, line %d: '%s'\n", __LINE__, p);
                    return;
                }
            } else if(*p == '!'){
                if( tp.type != PART_ARG && tp.size > 0){
                    vtpl.push_back(tp);
                    tp.data.clear();
                    tp.size = 0;
                }
                tp.type = PART_ARG;

                p++; // skip first question mark
                if(*p != '!'){
                    printf("[!] invalid template, line %d: '%s'\n", __LINE__, p);
                    return;
                }
                p++; // skip second question mark
                tp.size++;

                if(!*p) break; // EOTPL
                if(*p != ' '){
                    printf("[!] invalid template, line %d: '%s'\n", __LINE__, p);
                    return;
                }
            } else {
                if( tp.type != PART_DATA && tp.size>0 ){
                    vtpl.push_back(tp);
                    tp.data.clear();
                    tp.size = 0;
                }
                tp.type = PART_DATA;

                c = strtoul(p,&p1,0x10) & 0xff;
                if( p1 == p ) {
                    printf("[!] invalid template, line %d: c=%02x: '%s'\n", __LINE__, c, p);
                    return;
                }
                tp.data += c;
                tp.size++;
                if( !*p) break;

                p = p1;
            }
            while(*p && (*p == ' ') || (*p == '\n') || (*p == '\r') || (*p == '\t')) p++;
        }

        if( tp.size > 0 ) vtpl.push_back(tp);
    }

    uint32_t size(){
        uint32_t r = 0;
        for(int i=0; i<vtpl.size(); i++){
            r += vtpl[i].size;
        }
        return r;
    }

    void dump(){
        printf(" === template dump \n");
        for(int i=0; i<vtpl.size(); i++){
            printf("[d] %2d: type=%d size=%d\n", i, vtpl[i].type, vtpl[i].size);
        }
    }

    char* find(char*region_start, char*region_end){
        int i;

        if( region_end <= region_start ) return NULL;

        for(char*p = region_start; p < region_end; p+=align ){
            char* p1=p;
            results.clear();
            uint32_t result;
            bool stop = false;

            for(i=0; i<vtpl.size() && !stop; i++){
                switch( vtpl[i].type ){
                    case PART_DATA:
                        if(!memcmp(vtpl[i].data.data(), p1, vtpl[i].size)){
                            if(debug) printf("[d] matched %d bytes at %p\n", vtpl[i].size, p1);
                            p1 += vtpl[i].size;
                        } else {
                            stop = true; // compare failed
                        }
                        break;

                    case PART_ARG:
                        result = 0;
                        memcpy(&result, p1, min(4,vtpl[i].size));
                        results.push_back(result);
                        p1 += vtpl[i].size;
                        break;

                    case PART_SKIP:
                        if(debug>1) printf("[d] skip %d bytes at %p\n", vtpl[i].size, p1);
                        p1 += vtpl[i].size;
                        break;

                    default:
                        printf("[!] unknown tpl part type %d !\n", vtpl[i].type);
                        return NULL;
                }
            }
            if( i >= vtpl.size() && !stop ){
                if( debug) printf("[d] i=%d at %p\n", i, p);
                // full match!
                return p;
            }
        }

        return NULL;
    }

    int resultCount(){
        return results.size();
    }

    uint32_t getResult(int i){
        if( i >= 0 && i < results.size()){
            return results[i];
        }
        printf("[!] invalid index %d in BinaryTemplate::getResult\n", i);
        return 0;
    }
};
