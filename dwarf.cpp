#include "creature.cpp"

class Dwarf : public Creature {
    public:

    // returns only ALIVE DWARVES
    static Dwarf* getNext(int*idx){
        return (Dwarf*)Creature::getNext(idx, RACE_DWARF);
    }
};
