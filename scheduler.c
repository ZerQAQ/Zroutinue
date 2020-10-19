#include "type.h"
#include "list.h"
#include "asm_marco.h"
#include "const.h"
#include "var_arg.h"

#include <stdlib.h>
#include <stdarg.h>

/* typedef struct{
    u64 routinue_num, routinue_cap, running;
    __Context **routinue_ctxs;
    __ListNode *ready, *waiting;

    u8 *rbp, *rsp, *stack;
} __Scheduler; */

/* typedef struct {
    u8 *stack;
    u64 addr, stack_size, rbp, flag;
} __Context; */

__Scheduler __S_zerqaq;

void __sch_init(){
    __S_zerqaq.routinue_num = 0;
    __S_zerqaq.routinue_cap = init_routinue_num;
    __S_zerqaq.routinue_ctxs = malloc(sizeof(void*) * init_routinue_num);

    list_init(__S_zerqaq.ready);
    list_init(__S_zerqaq.waiting);

    __S_zerqaq.routinue_stack = malloc(routinue_stack_size);
    __S_zerqaq.routinue_s_base = (u64)(__S_zerqaq.routinue_stack + routinue_stack_size - 128) & 0xffffffffffffff00;
}

#define __go(...) var_args_func(__go_raw, __VA_ARGS__)

void __go_raw(u64 arg_num, void* func_ptr, ...){
    __Context *c = malloc(sizeof(__Context));
    __S_zerqaq.routinue_ctxs[++__S_zerqaq.routinue_num] = c;
    list_add(__S_zerqaq.ready, __S_zerqaq.routinue_num, __la_1);
    
    c->id = __S_zerqaq.routinue_num;
    c->addr = func_ptr;
    c->stack_size = 0;
    c->stack = NULL;

    c->arg_num = arg_num - 1;
    c->args = malloc(arg_num - 1);

    va_list vl;
    va_start(vl, func_ptr);
    for(int i = 0; i < arg_num - 1; i++) c->args[i] = va_arg(vl, u64);
    va_end(vl);
}

void __sch_recy(__Context *ctx);

//第一次启动的协程
void __sch_start(__Context *ctx){
    u64 *rsp = __S_zerqaq.routinue_s_base - 1;
    if(ctx->arg_num > 6){
        for(int i = ctx->arg_num - 1; i > 6; i--){
            *(++rsp) = ctx->args[i];
        }
    }
    *(++rsp) = __sch_recy;

#define __cek_and_set_reg(i, des_reg) if(ctx->arg_num >= i) set_reg(des_reg, ctx->args[i - 1]);
    __cek_and_set_reg(1, rdi);
    __cek_and_set_reg(2, rsi);
    __cek_and_set_reg(3, rdx);
    __cek_and_set_reg(4, rcx);
    __cek_and_set_reg(5, r8);
    __cek_and_set_reg(6, r9);
    
    set_reg(rsp, rsp);
    jmp(ctx->addr);
}

void __sch_recy(__Context *ctx){
    printf("a routinue is end.\n");
    exit(0);
}