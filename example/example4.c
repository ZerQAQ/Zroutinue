#include "Zroutinue.h"

void unsafe_1(){
    Channel ch = mkch(double);
    chclose(ch);
    chwrite(ch, 1.0);
}

void unsafe_2(){
    Channel ch = mkch(int, 5);
    chwrite(ch, 1LU);
}

void entry(){
    unsafe_1();
    //output: example4.c:6: Channel: you are trying to write to a closed channel

    unsafe_2();
    //output: example4.c:10: Channel: you are trying to write a wrong type to channel
}

int main(){
    Zroutinue_init(entry);
}