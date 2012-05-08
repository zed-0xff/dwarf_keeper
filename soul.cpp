#include "common.h"
#include "skill.cpp"

typedef vector<Skill*> SkillsVector;

class Soul : public MemClass {

    public: 
    SkillsVector* getSkillsVector(){
        if( GAME.soul_skills_offset ){
            return (SkillsVector*)checked_vector(GAME.soul_skills_offset);
        } else {
            return NULL;
        }
    }

    //////////////////////////////////////////////////////////////////

    private:
    //static const int      SKILLS_VECTOR_OFFSET =  0x1c4; // DF 31.24
    //static const int      SKILLS_VECTOR_OFFSET =  0x1d4; // DF 37.04
};
