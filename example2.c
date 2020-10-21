#include "src/Zroutinue.h"

#include <stdio.h>
#include <stdlib.h>

void _f(Channel ch_in, Channel ch_out){
    int temp;
    chread(ch_in, temp);
    chwrite(ch_out, temp);
    chfree(ch_in);
}

// 输入一个旧channel，返回一个新channel，并开启一个协程，将旧channel的值传给新channel
Channel f(Channel ch){
    Channel ret = mkch(int);
    go(_f, ch, ret);
    return ret;
}

//开启和销毁n个channel
void entry(int n){
    Channel ch = mkch(int);
    Channel start = ch;
    for(int i = 1; i < n; i++) ch = f(ch);
    chwrite(start, 0x1234);
    int temp;
    chread(ch, temp);
    printf("%x\n", temp);
    //output: 1234
}
//在笔者机器上仅用了0.1秒就完成了10万个协程的开启与创建

int main(int argn, char **argv){
    if(argn < 2) {printf("need param, usage: %s <routinue_number>\n", argv[0]); return 0;}
    int val = atoi(argv[1]);
    Zroutinue_init(entry, val);
}