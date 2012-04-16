#include "common.h"

typedef vector<Item*> ItemsVector;

class Item{
    public:
    void *pvtbl;    // pointer to vtable

    static const int SIZE = 0x90;

    string getName(){
        string s;
        getItemName(this, &s, 0, -1);
        return s;
    }

    string getBaseName(int mode){
        // mode 0: dimple cups [3]
        // mode 1: dimple cup
        // mode 2: dimple cups
        string s;
        getItemBaseName(this, &s, mode);
        return s;
    }

    int getValue(){
        return getItemValue(this, 0, 0);
    }

    int getId(){
        return *(int*)((char*)this+ID_OFFSET);
    }

    static ItemsVector* getVector(){
        return (ItemsVector*)ITEMS_VECTOR;
    }

    private:
    static const int ID_OFFSET = 0x14;
};
