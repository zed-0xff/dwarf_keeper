#include <vector>
#include <iostream>

using namespace std;

class Foo;

vector<Foo> foofunc(){
}

class Foo{
    public:
    int bar,baz;
    Foo(int b){
        bar = b;
    }

    void foo(){
        printf("I'm a foo %p %d %d\n", this, bar, *(int*)this);
    }
};


int main(int argc, char*argv){
    vector<Foo> v(4,100);
    int*p,*p0;

    vector<Foo>::iterator itr;
    for ( itr = v.begin(); itr < v.end(); ++itr ) {
        itr->foo();
    }
    puts("");

    printf("v ptr = %p, size = %d\n",&v, sizeof(v));
    for(p=(int*)&v; p<(int*)(&v+sizeof(v)/4); p++){
        printf("v[%p] = %x\n", p, *p);
    }
    puts("");
    p = (int*)&v;
    printf("%x %x\n", *p, *(p+1));
    for(p=*(int**)&v; p < *(int**)((char*)&v+4); p++){
        printf("v[%p] = %x\n", p, *p);
    }
}
