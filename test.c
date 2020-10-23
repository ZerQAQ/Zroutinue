#include "src/Zroutinue.h"

#include <stdio.h>

void entry(u64 a, u32 b, u16 c, short *d){
    printf("%llx %llx %llx %llx", a, b, c, d);
}

int main(){
    entry(0x101, 0x102, 0x3, 0x104);
    Zroutinue_init(entry, 0x101, 0x102, 0x103, 0x104);
}