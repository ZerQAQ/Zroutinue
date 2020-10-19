#include "asm_marco.h"
#include "type.h"
#include "scheduler.c"

#include <stdio.h>

typedef struct{
    u64 val;
    double a, b, c;
} T;

#define p(i) printf("%llx ", a##i);
#define _p(i) printf("%p ", a##i);

void test(u32 a1, u32 a2, u16 a3,u32 a4, u32 a5, u8 a6, u8 a7, u8 a8, u8 a9, u8 a10){
    p(1); p(2); p(3); p(4); p(5); p(6); p(7); p(8); p(9); p(10); puts("\n");

    u64 rbp;
    get_reg(rbp, rbp);
    u64 *ptr = rbp;
    for(int i = 0; i < 10; i++){
        printf("%llx ", *ptr);
        ptr++;
    }
}


int main(){
    __s_init();
    __Context *c = __go(test, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110);
}