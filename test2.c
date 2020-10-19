#include<stdio.h>
#include<stdlib.h>

#include "asm_marco.h"

typedef unsigned char u8;
typedef unsigned long long u64;
typedef unsigned int u32;

u64 f_addr;
const u8 s[] = "wow!\n";
void __f(){
    __asm__ __volatile__ (
        "leaq (%%rip), %0\n\t"
        :"=r"(f_addr)
    );
    __asm__ __volatile__ (
        "_JUMP:\n\t"
    );
    printf("in __f: %p %llx\n", __f, f_addr);
    printf((const char*)s);
    static int c = 0;
    if(c == 1) exit(0);
    c++;
    return;
}

u8 stack[10240], *stack_base = stack + 10240 - 128;

int main(){

    u64 rip;
    __asm__ __volatile__ (
        "leaq (%%rip), %0\n\t"
        "nop"
        :"=r"(rip)
    );
    printf("%llx\n", rip);

    return 0;

    __f();

    u8 s[] = "main's stack";
    u64 rsp, rbp;
    get_reg(rsp, rsp);
    get_reg(rbp, rbp);
    u64 stack_size = rbp - rsp;

    u8 *temp = stack_base - stack_size;

    u8 *_rsp = (u8*)rsp;
    for(int i = 0; i < stack_size; i++) printf("%02x ", *(_rsp + i));
    putchar('\n');

    copy_stack(temp);

    for(int i = 0; i < stack_size; i++) printf("%02x ", *(stack_base - stack_size + i));
    putchar('\n');
    
    set_reg(rbp, stack_base);
    mv_sb_rsp(stack_size);

    *(u64*)(stack_base + 8) = f_addr;

    return 0;
}