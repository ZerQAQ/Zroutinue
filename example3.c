#include "Zroutinue.h"
/* 
#include "scheduler.c"
#include "channel.c"
#include "list.c" 
*/

#include <stdio.h>

void _f(Channel ch_in, Channel ch_out){
    int temp;
    chread(ch_in, temp);
    chwrite(ch_out, temp);
}

Channel f(Channel ch){
    Channel ret = mkch(int);
    go(_f, ch, ret);
    return ret;
}

void entry(int n){
    Channel ch = mkch(int);
    Channel start = ch;
    for(int i = 1; i < n; i++) ch = f(ch);
    chwrite(start, 0x1234);
    int temp;
    chread(ch, temp);
    printf("%x\n", temp);
}

int main(int argn, char **argv){
    if(argn < 2) {printf("need param, usage: %s <prim_num>\n", argv[0]); return 0;}
    int val = atoi(argv[1]);
    Zroutinue_init(entry, val);
}