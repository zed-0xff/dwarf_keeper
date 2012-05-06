#include "common.h"

// see reversing/skill_type.rb
static const int SKILL_TYPE_MAP[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,1,2,1,1,1,1,1,2,3,3,2,2,2,2,2,2,3,2,2,2,2,2,2,3,3,3,3,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,2,3,2,2,2,2,2,2,2};

class Skill : public MemClass {
    public:

    // skill types
    static const int TYPE_INVALID = 0;
    static const int TYPE_COMBAT  = 1;
    static const int TYPE_LABOR   = 2;
    static const int TYPE_MISC    = 3;

    int getId(){    return  w(ID_OFFSET);    }
    int getLevel(){ return dw(LEVEL_OFFSET); }
    int getType() {
        int id = getId();
        if(id >= 0 && id < sizeof(SKILL_TYPE_MAP)/sizeof(SKILL_TYPE_MAP[0])){
            return SKILL_TYPE_MAP[id];
        } else {
            return TYPE_INVALID;
        }
    }

    string nameString(int race, int sex){
        string s;
        ((func_t_siii)(SKILL_ID_2_S_FUNC))( &s, getId(), race, sex );
        return cp437_to_utf8(s);
    }

    string levelString(){
        string s;
        ((func_t_si)(SKILL_LVL_2_S_FUNC))( &s, getLevel() );
        return cp437_to_utf8(s);
    }

    //////////////////////////////////////////////////////////////////

    private:
    static const int      ID_OFFSET = 0; // word
    static const int   LEVEL_OFFSET = 4; // dword
};

