#include "unit.cpp"

class Dwarf : public Unit {
    public:

    // returns only ALIVE DWARVES
    static Dwarf* getNext(int*idx){
        return (Dwarf*)Unit::getNext(idx, RACE_DWARF);
    }
};
