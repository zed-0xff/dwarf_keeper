#ifndef GAME_H
#define GAME_H

#include <vector>

using namespace std;

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
//    void *unit_type_func;   // (pUnit)

    uint32_t unit_soul_offset;      // offset of Soul* in Unit
    uint32_t unit_happiness_offset;
    uint32_t unit_phys_attrs_offset;
    uint32_t unit_wearings_vector_offset;
    uint32_t unit_job_offset;

    uint32_t item_id_offset;
    uint32_t item_refs_vector_offset;

    uint32_t soul_skills_offset;    // offset of skills vector in Soul

    uint32_t bld_vtbl_getname_offset;
    uint32_t bld_vtbl_getvalue_offset;

    // int job_name(Job*, string*)
    void *job_name_func;

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

#endif
