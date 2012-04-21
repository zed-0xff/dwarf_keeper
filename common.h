#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class Item;
class Dwarf;
class HTTPRequest;

#define CREATURES_VECTOR        0x157e5f8
#define ITEMS_VECTOR            0x157e668

#define CREATURE_FULL_NAME_FUNC 0x96b030
#define DWARF_THOUGHTS_FUNC     0xa1aa60 // (pDwarf, string*)

#define ITEM_BASE_NAME_FUNC     0x612c10 // item name w/o any modifiers
#define ITEM_NAME_FUNC          0x613730 // full item name w/all quality/condition/etc modifiers
#define ITEM_VALUE_FUNC         0x604580

// int cmp_item_size(int itemType, int itemSubType, int race_id_1, int race_id_2)
#define CMP_ITEMSIZE_FUNC       0x947760 

typedef int(*info_func3_t)(void*, string*, int);
typedef int(*info_func4_t)(void*, string*, int, int);
typedef int(*value_func_t)(void*, int, int);

typedef int(*no_arg_func_t)();
typedef int(*pvoid_arg_func_t)(void*);
typedef int(*func_t_4_ints)(int,int,int,int);
typedef int(*func_t_2_pvoids)(void*, void*);

info_func3_t getCreatureFullName = (info_func3_t)CREATURE_FULL_NAME_FUNC;
info_func4_t getItemName         = (info_func4_t)ITEM_NAME_FUNC;
info_func3_t getItemBaseName     = (info_func3_t)ITEM_BASE_NAME_FUNC;
value_func_t getItemValue        = (value_func_t)ITEM_VALUE_FUNC;

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

static struct timeval g_t0,g_t1;
#define BENCH_START      gettimeofday(&g_t0, NULL);
#define BENCH_END(title) gettimeofday(&g_t1, NULL); printf("[t] %3d %s\n", diff_ms(g_t1, g_t0), title);

#include "dwarf.cpp"
#include "item.cpp"
#include "html.cpp"
#include "unicode.cpp"
#include "http_request.cpp"

#endif