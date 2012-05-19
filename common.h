#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <map>
#include <set>

// tricky includes to include platform.h from both microhttpd and g_src
#include <microhttpd.h>
#include "g_src/platform.h"
#include "g_src/graphics.h"

// HACK: need microhttpd internals to be able to extract socket_fd from MHD_Connection
#define MHD_PLATFORM_H
#include "libmicrohttpd/src/daemon/internal.h"

using namespace std;

class Item;
typedef vector<Item*> ItemsVector;

class Dwarf;
class HTTPRequest;

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

static struct timeval g_t0,g_t1;
#define BENCH_START      gettimeofday(&g_t0, NULL);
#define BENCH_END(title) gettimeofday(&g_t1, NULL); printf("[t] %3d %s\n", diff_ms(g_t1, g_t0), title);

#include "x86_emu.cpp"

// all struct members will be automatically zero-initialized
//   http://ex-parrot.com/~chris/random/initialise.html
//   http://bytes.com/topic/c/answers/832184-struct-member-initialization
static struct {
    void *units_vector, *items_vector, *buildings_vector;
    void *item_name_func, *item_base_name_func, *item_value_func;

    void *unit_name_func;
    void *unit_info_func;   // (pUnit, string*)
    void *unit_coords_func; // (pUnit, int *px, int *py, int *pz)

    uint32_t unit_soul_offset;      // offset of Soul* in Unit
    uint32_t unit_happiness_offset;
    uint32_t soul_skills_offset;    // offset of skills vector in Soul

    // int cmp_item_size(int itemType, int itemSubType, int race_id_1, int race_id_2)
    void *cmp_item_size_func;

    // int skill_lvl_2_string(string*, int level)
    void *skill_lvl_2_string_func;

    // int skill_id_2_string(string*, int skill_id, int race, int sex)
    void *skill_id_2_string_func;

    // int setScreenCenter(int center_mode) - actual coords are set via following global variables
    void *set_screen_center_func;
    int  *scr_target_center_px, *scr_target_center_py, *scr_target_center_pz;

    void *unit_info_right_panel_func;

    void* root_screen;

    void* offscr_renderer_ctor_func;
    void* offscr_renderer_render_func;
    void* offscr_renderer_dtor_func;

    vector <mem_write_t> unit_info_right_panel_mem_writes;
} GAME = {0};

#define GAME_INFO_LAST_PTR &GAME.offscr_renderer_dtor_func

#include "binary_template.cpp"

#ifdef __linux__
#include "_linux.h"
#else
#include "_osx.h"
#endif

typedef int(*no_arg_func_t)();
typedef int(*pvoid_arg_func_t)(void*);
typedef int(*func_t_4_ints)(int,int,int,int);
typedef int(*func_t_2_pvoids)(void*, void*);
typedef int(*func_t_si)(string*, int);
typedef int(*func_t_siii)(string*, int, int, int);
typedef int(*func_t_i)(int);
typedef int(*func_t_p)(void*);
typedef int(*func_t_pi)(void*, int);
typedef int(*func_t_pp)(void*, void*);
typedef int(*func_t_ppii)(void*, void*, int, int);
typedef int(*func_t_cc)(const char*, const char*);
typedef int(*func_t_pii)(void*, int, int);
typedef int(*func_t_ppi)(void*, void*, int);
typedef int(*func_t_pppp)(void*, void*, void*, void*);

int str_replace(string&s, const char*from, const char*to){
    int i = 0;
    int lto = strlen(to);
    int lfrom = strlen(from);
    int nreplaces = 0;

    while( (i=s.find(from, i)) != string::npos ){
        s.replace(i, lfrom, to);
        i += lto-lfrom+1;
        nreplaces++;
    }

    return nreplaces;
}

int str_replace(string&s, const char*from,const string&to){
    int i = 0;
    int lto = to.size();
    int lfrom = strlen(from);
    int nreplaces = 0;

    while( (i=s.find(from, i)) != string::npos ){
        s.replace(i, lfrom, to);
        i += lto-lfrom+1;
        nreplaces++;
    }

    return nreplaces;
}

int str_replace(string&s, const string&from,const string&to){
    int i = 0;
    int lto = to.size();
    int lfrom = from.size();
    int nreplaces = 0;

    while( (i=s.find(from, i)) != string::npos ){
        s.replace(i, lfrom, to);
        i += lto-lfrom+1;
        nreplaces++;
    }

    return nreplaces;
}



#include "unicode.cpp"
#include "mem_class.cpp"
#include "dwarf.cpp"
#include "item.cpp"
#include "html.cpp"
#include "http_request.cpp"

#endif
