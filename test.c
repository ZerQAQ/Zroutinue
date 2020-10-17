#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "asm_marco.h"
#include "type.h"
#include "const.h"

u8 __buf[1000];

void __temp(__Context *ctx){

}

//callee saved register in x86-64:
//rbx rbp r12 r13 r14 r15
void __save_ctx(u64 bp){
    __Context *ctx = malloc(sizeof(__Context));
    u8 s[] = "yield's stack";
    __asm__ __volatile__(
        "mov %%rbx, %1\n\t"
        "subq %%rsp, %2\n\t"
        "movq %2, %0\n\t"
        :"=r"(ctx->stack_size), "=r"(ctx->rbp)
        :"r"(bp)
        :"memory"
    );
    ctx->stack = malloc(ctx->stack_size);
    __asm__ __volatile__(
        "movq %%rsp, %%rsi\n\t"
        "rep movsb\n\t"
        ::"D"(ctx->stack), "c"(ctx->stack_size)
    ); 
    for(int i = 0; i < ctx->stack_size; i++){
        putchar(ctx->stack[i]);
    } putchar('\n');
    __asm__ __volatile__(
        "leaq (%%rip), %0\n\t"
        "addq $20, %0\n\t"
        :"=r"(ctx->addr)
    );
    __temp(ctx);

    printf("%llx %llx\n",ctx->stack_size, ctx->addr);
}

#define save_ctx()\
    u64 __save_ctx_bp;\
    __asm__ __volatile__(\
        "movq %%rbp, %0\n\t"\
        :"=r"(__save_ctx_bp)\
    );\
    __save_ctx(__save_ctx_bp);


u8 __stack[1000];
void recover_ctx(__Context *ctx){

}

#define __sch_stack_size 10240
u8 __sch_stack[__sch_stack_size], *__sch_stack_base = __sch_stack + __sch_stack_size - 32;
u8 __coroutinue_stack[__sch_stack_size], *__coroutinue_stack_base = __coroutinue_stack + __sch_stack_size - 32;
u64 __sch_rbp, __sch_rsp;

u64 __sch_routinue_end_addr;

__Scheduler __S_zerqaq;

void scheduler(){
    u8 s[] = "sch's stack";
__sch_init:
    get_reg(rbp, __sch_rbp);
    get_reg(rsp, __sch_rsp);
    __sch_rsp = (u64)__sch_stack_base + __sch_rsp - __sch_rbp;
    __sch_rbp = (u64)__sch_stack_base;
    copy_stack(__sch_stack);

    __S_zerqaq.routinue_num = 0;
    __S_zerqaq.routinue_ctxs = malloc(sizeof(void*) * init_routinue_num);
    
    //for(int i = 0; i < 50; i++) putchar(__sch_stack[i]);

    #define __sch_recover()\
        set_reg(rbp, __sch_rbp);\
        set_reg(rsp, __sch_rsp);

    #define __sch_function_call(func)\
        set_reg(rbp, __coroutinue_stack_base);\
        get_rip(* (u64*) (__coroutinue_stack + __coroutinue_stack_base + 8));\
        

__sch_start:


//__sch_schedul:
    label(__sch_schedul);

// __sch_croutine_end
    get_rip(__sch_routinue_end_addr);
    __sch_recover();
    printf("a croutine is end\n");
    exit(1);
}

void f1(){
    printf("f1_1");
    save_ctx();
    printf("f1_2");
}

void f2(){
    printf("f2_1");
    save_ctx();
    printf("f2_2");
}

int main()
{
    u8 s[] = "main's stack";
    scheduler();
    //test();

    /* long long test;
    long long *ptr = &test;
    __asm__ __volatile__(
        "movq %%rbp, %%rcx\n\t"
        "subq %%rsp, %%rcx\n\t"
        "movq %%rcx, (%0)\n\t"
        "movq %%rsp, %%rsi\n\t"
        "rep movsb"
        :
        :"b"(ptr), "D"(__buf)
        :"memory"
    );
    printf("%llx %p ", test, __buf);
    for(long long i = 0; i < test; i++) putchar(__buf[i]);
    putchar('\n'); */
    return 0;
}