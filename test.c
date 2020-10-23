#include "src/Zroutinue.h"

#include <stdio.h>

void entry(u64 a, u32 b, u16 c, short *d, u8 e, u8 f, u8 g){
    printf("%llx %llx %llx %llx %llx %llx %llx\n", a, b, c, d, e, f, g);
}

void _entry(u64 a, u32 b, u16 c, short *d){
    printf("%llx %llx %llx %llx\n", a, b, c, d);
}

int main(){
    _entry(0x10, 0x11, 0x12, 0x13);
    entry(0x101, 0x102, 0x103, 0x104, 1, 2, 3);
    Zroutinue_init(entry, 0x101, 0x102, 0x103, 0x104, 1, 2, 3);
}