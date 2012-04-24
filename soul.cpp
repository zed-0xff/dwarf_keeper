#include "common.h"
#include "skill.cpp"

typedef vector<Skill*> SkillsVector;

class Soul : public MemClass {

    public: 
    SkillsVector* getSkillsVector(){
        return (SkillsVector*)((char*)this + SKILLS_VECTOR_OFFSET);
    }

    //////////////////////////////////////////////////////////////////

    private:
    static const int      SKILLS_VECTOR_OFFSET =  0x1d4;
};
