#include<stdlib.h>
#include<stdio.h>

#include "type.h"

const void* malloc_addr = malloc;

int get_val(int val){
    return 0x100 * val;
}

void jump(){
    __asm__ __volatile__(
        "__JMP:"
    );
    printf("wow");
    exit(1);
}

int main(){
    __asm__ __volatile__ (
        "jmp __JMP"
    );
    u8 *p = NULL;
    /* __asm__ __volatile__ (
        "movq $0x6, %rdi\n\t"
    );
    ((void*(*)())get_val)();
    __asm__ __volatile__ (
        "movq %%rax, %0"
        :"=m"(p)
    ); */
    p = malloc(1024);
    printf("%p", p);
}