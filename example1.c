#include "Zroutinue.h"

#include <stdio.h>
#include <stdlib.h>

void _generator(Channel ch){
    int val = 2;
    for(;;){
        chwrite(ch, val);
        val++;
    }
}

//返回一个channel, 并开启一个协程往channel中不断传入从2开始的自然数
Channel make_generator(){
    Channel ch = mkch(int);
    go(_generator, ch);
    return ch;
}

void _filter(Channel ch_in, Channel ch_out, int k){
    int val;
    for(;;){
        chread(ch_in, val);
        while(val % k == 0) chread(ch_in, val)
        chwrite(ch_out, val);
    }
}

//读入一个旧channel，返回一个新channel，并开启一个协程，这个协程从旧channnel得到的数据，过滤掉k的倍数，传入新的channel
Channel make_filter(Channel ch, int k){
    Channel ret = mkch(int);
    go(_filter, ch, ret, k);
    return ret;
}

//打印n个质数
void prim_generator(int n){
    Channel gen = make_generator();
    int prim;
    for(int i = 0; i < n; i++){
        chread(gen, prim);
        printf("%d ", prim);
        gen = make_filter(gen, prim);
    }
    putchar('\n');
}

int main(int argn, char** argv){
    if(argn < 2) {printf("need param, usage: %s <prim_num>\n", argv[0]); return 0;}
    int val = atoi(argv[1]);
    Zroutinue_init(prim_generator, val);
}

/*
output:
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 ...
*/