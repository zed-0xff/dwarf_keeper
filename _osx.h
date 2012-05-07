typedef int(*unit_name_func_t)(void*, string*, int);

unit_name_func_t getUnitFullName = NULL;

#define UNITS_VECTOR            0x157e5f8

#define UNIT_FULL_NAME_FUNC     0x96b030

#define ITEM_VALUE_FUNC         0x604580

#define ITEMS_VECTOR            0x157e668

#define ITEM_BASE_NAME_FUNC     0x612c10 // item name w/o any modifiers
#define ITEM_NAME_FUNC          0x613730 // full item name w/all quality/condition/etc modifiers

#define BUILDINGS_VECTOR        0x157ecc8

#define DWARF_THOUGHTS_FUNC     0xa1aa60 // (pDwarf, string*)

// int cmp_item_size(int itemType, int itemSubType, int race_id_1, int race_id_2)
#define CMP_ITEMSIZE_FUNC       0x947760 

// int skill_lvl_2_string(string*, int level)
#define SKILL_LVL_2_S_FUNC      0x209e80

// int skill_id_2_string(string*, int skill_id, int race, int sex)
#define SKILL_ID_2_S_FUNC       0x20b170

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

void os_init(){
    units_vector        = (void*) UNITS_VECTOR;
    items_vector        = (void*) ITEMS_VECTOR;
    buildings_vector    = (void*) BUILDINGS_VECTOR;

    getUnitFullName     = (info_func3_t)UNIT_FULL_NAME_FUNC;
    getItemName         = (info_func4_t)ITEM_NAME_FUNC;
    getItemBaseName     = (info_func3_t)ITEM_BASE_NAME_FUNC;
}
