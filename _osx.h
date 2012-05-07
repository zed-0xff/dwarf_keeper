typedef int(*unit_name_func_t)(void*, string*, int);

unit_name_func_t getUnitFullName = NULL;

#define UNITS_VECTOR            0x157e5f8

#define UNIT_FULL_NAME_FUNC     0x96b030

#define ITEM_VALUE_FUNC         0x604580

#define ITEMS_VECTOR            0x157e668

#define ITEM_BASE_NAME_FUNC     0x612c10 // item name w/o any modifiers
#define ITEM_NAME_FUNC          0x613730 // full item name w/all quality/condition/etc modifiers

#define BUILDINGS_VECTOR        0x157ecc8

void os_init(){
    units_vector        = (void*) UNITS_VECTOR;
    items_vector        = (void*) ITEMS_VECTOR;
    buildings_vector    = (void*) BUILDINGS_VECTOR;

    getUnitFullName     = (info_func3_t)UNIT_FULL_NAME_FUNC;
    getItemName         = (info_func4_t)ITEM_NAME_FUNC;
    getItemBaseName     = (info_func3_t)ITEM_BASE_NAME_FUNC;
}
