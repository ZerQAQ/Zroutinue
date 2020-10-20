#include "asm_marco.h"
#include "type.h"

u64 main_rsp, main_rbp;

#include "scheduler.c"
#include "channel.c"

#include <stdio.h>

typedef struct{
    u64 val;
    double a, b, c;
} T;

#define p(i) printf("%llx ", a##i);
#define _p(i) printf("%p ", a##i);

void test(u32* a1, u32 a2, u16 a3,u32 a4, u32* a5, u16 a6, u16 a7, u16 a8, u16* a9, u16 a10, u64 a11, u64 a12){

    puts("in test:");
    p(1); p(2); p(3); p(4); p(5); p(6); p(7); p(8); p(9); p(10); p(11); p(12); puts("\n");

    u64 rbp;
    get_reg(rbp, rbp);
    printf("in test: rbp: %llx", rbp);
    u64 *ptr = rbp;
    printf("%p\n", rbp);
    for(int i = 0; i < 10; i++){
        printf("%llx ", *ptr);
        ptr++;
    } printf("\n");

    //__sch_save_ctx();

    u64 rsp;
    get_reg(rsp, rsp);
    printf("in test: rsp: %llx", rsp);
}

u64 temp;

#define prt_stack(func_name)\
    get_reg(rbp, rbp);\
    get_reg(rsp, rsp);\
    printf("%s:\n", #func_name);\
    printf("rsp:%p rbp:%p size: %lld\n", rsp, rbp, rbp - rsp);\
    for(__i = 0; __i < (rbp - rsp); __i++){\
        if(__i % 4 == 0) putchar(' ');\ 
        printf("%02x", *(rsp + __i));\
    } putchar('\n'); \


u8 *rsp, *rbp;
u64 __i;

int count(double val){
    __sch_save_ctx();
    return (int)val + 10;
}

void producer(int i){
    int val = 10 * i;
    for(int i = 0; i < 10; i++){
        temp = val;
        printf("%d is produced\n", temp);
        val = count((double)val);
    }
}

void consumer(){
    for(int i = 0; i < 10; i++){
        printf("%d is consumed\n", temp);
        __sch_save_ctx();
    }
}

void entry(){
    u8 *rbp, *rsp;

    get_reg(rbp, rbp);
    get_reg(rsp, rsp);

    /*puts("entry:");
    printf("rsp:%p rbp:%p\n", rsp, rbp);
    for(u64 i = 0; i < (rbp - rsp); i++){
        printf("%x ", *(rsp + i));
    } putchar('\n'); */

    printf("hello!\n");
    __go(producer, 10);
    __go(consumer);
    __sch_save_ctx();
    printf("hello!\n");
}

int main(){
    u64 rsp;
    get_reg(rsp, rsp);
    get_reg(rsp, main_rsp);
    get_reg(rbp, main_rbp);

    printf("main rsp: %llx\n", rsp); 
    //test(0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x110, 0x111, 0x112);

    __sch_init();
    //__Context *c = __go(test, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x110, 0x111, 0x112);

    __go(entry);
    __sch_scheduler();
}