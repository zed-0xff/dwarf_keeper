#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class Item;
typedef vector<Item*> ItemsVector;

class Dwarf;
class HTTPRequest;

#define UNITS_VECTOR            0x157e5f8
#define ITEMS_VECTOR            0x157e668
#define BUILDINGS_VECTOR        0x157ecc8

#define UNIT_FULL_NAME_FUNC     0x96b030
#define DWARF_THOUGHTS_FUNC     0xa1aa60 // (pDwarf, string*)

#define ITEM_BASE_NAME_FUNC     0x612c10 // item name w/o any modifiers
#define ITEM_NAME_FUNC          0x613730 // full item name w/all quality/condition/etc modifiers
#define ITEM_VALUE_FUNC         0x604580

// int cmp_item_size(int itemType, int itemSubType, int race_id_1, int race_id_2)
#define CMP_ITEMSIZE_FUNC       0x947760 

// int skill_lvl_2_string(string*, int level)
#define SKILL_LVL_2_S_FUNC      0x209e80

// int skill_id_2_string(string*, int skill_id, int race, int sex)
#define SKILL_ID_2_S_FUNC       0x20b170

// int foo_func(int unit_id)
#define FOO_FUNC 0x3021f0 // open unit info right panel?

// int getUnitCoords(Unit *pc, int *px, int *py, int *pz)
#define UNIT_COORDS_FUNC    0x949af0

// int setScreenCenter(int center_mode) - actual coords are set via following global variables
#define SET_SCREEN_CENTER_FUNC  0x2d5240

// global variables for SET_SCREEN_CENTER_FUNC
#define SCR_TARGET_CENTER_X     0x1563a30
#define SCR_TARGET_CENTER_Y     0x1563a34
#define SCR_TARGET_CENTER_Z     0x1563a38

//#define PSCREENS_LIST            0x17ce5c8 // ptr to double-linked list of active screens
#define ROOT_SCREEN             0x0e27044

//#define PROOT_WINDOW             0x17ce5c0 // ptr to root window, always 0x017c8720
#define ROOT_WINDOW             0x017c8720

#define OFFSCR_RENDERER_CTOR    0x0cc16a0
#define OFFSCR_RENDERER_RENDER  0x0cc1990
#define OFFSCR_RENDERER_DTOR    0x0cc1820

typedef int(*info_func3_t)(void*, string*, int);
typedef int(*info_func4_t)(void*, string*, int, int);
typedef int(*value_func_t)(void*, int, int);

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
typedef int(*func_t_cc)(const char*, const char*);
typedef int(*func_t_pii)(void*, int, int);
typedef int(*func_t_ppi)(void*, void*, int);
typedef int(*func_t_pppp)(void*, void*, void*, void*);

info_func3_t getUnitFullName     = (info_func3_t)UNIT_FULL_NAME_FUNC;
info_func4_t getItemName         = (info_func4_t)ITEM_NAME_FUNC;
info_func3_t getItemBaseName     = (info_func3_t)ITEM_BASE_NAME_FUNC;
value_func_t getItemValue        = (value_func_t)ITEM_VALUE_FUNC;

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



int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

static struct timeval g_t0,g_t1;
#define BENCH_START      gettimeofday(&g_t0, NULL);
#define BENCH_END(title) gettimeofday(&g_t1, NULL); printf("[t] %3d %s\n", diff_ms(g_t1, g_t0), title);

#include "unicode.cpp"
#include "mem_class.cpp"
#include "dwarf.cpp"
#include "item.cpp"
#include "html.cpp"
#include "http_request.cpp"

#endif
