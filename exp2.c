#include "Zroutinue.h"

void f1(Channel ch){
    int temp;
    chread(ch, temp);
}

void f2(Channel ch){
    int temp;
    chread(ch, temp);
}

void entry(){
    Channel ch = mkch(int);
    go(f1, ch);
    go(f1, ch);
    go(f2, ch);
    int temp;
    chread(ch, temp);
}

int main(){
    Zroutinue_init(entry);
}