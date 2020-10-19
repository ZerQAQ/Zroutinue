#include "type.h"
#include "list.h"
#include "asm_marco.h"
#include "const.h"
#include "var_arg.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define __EXIT\
    set_reg(rsp, main_rsp);\
    set_reg(rbp, main_rbp);\
    exit(0);

__Scheduler __S_zerqaq;

void __sch_init(){
    __S_zerqaq.routinue_num = 0;
    __S_zerqaq.routinue_cap = init_routinue_num;
    __S_zerqaq.routinue_ctxs = malloc(sizeof(void*) * init_routinue_num);
    if(__S_zerqaq.routinue_ctxs == 0x0) perror("malloc error");

    list_init(__S_zerqaq.ready);
    if(__S_zerqaq.ready == 0x0) perror("malloc error");
    list_init(__S_zerqaq.waiting);
    if(__S_zerqaq.waiting == 0x0) perror("malloc error");

    __S_zerqaq.routinue_stack = malloc(routinue_stack_size);
    if(__S_zerqaq.routinue_stack == 0x0) perror("malloc error");
    __S_zerqaq.routinue_s_base = ((u64)(__S_zerqaq.routinue_stack + routinue_stack_size) & 0xfffffffffffff000UL) - 0x2000;
    printf("stack is between %p ~ %p", __S_zerqaq.routinue_stack, __S_zerqaq.routinue_stack + routinue_stack_size);
    puts("");
}

#define __go(...) var_args_func(__go_raw, __VA_ARGS__)

__Context* __go_raw(u64 arg_num, void* func_ptr, ...){
    //新建上下文
    __Context *c = malloc(sizeof(__Context));
    __S_zerqaq.routinue_ctxs[++__S_zerqaq.routinue_num] = c;

    //新建链表节点，放入ready队列
    __ListNode *node = malloc(sizeof(__ListNode));
    node->val = __S_zerqaq.routinue_num;
    list_add(__S_zerqaq.ready, node);

    //初始化上下文
    c->node_ptr = node;
    c->id = __S_zerqaq.routinue_num;
    c->addr = func_ptr;
    c->stack_size = 0;
    c->stack = NULL;
    c->flag = 0;
    c->rbp = NULL;

    c->arg_num = arg_num - 1;
    if(c->arg_num > 0) c->args = malloc(c->arg_num * sizeof(u64));
    else c->args = NULL;

    va_list vl;
    va_start(vl, func_ptr);
    for(int i = 0; i < c->arg_num; i++) c->args[i] = va_arg(vl, u64);
    va_end(vl);

    return c;
}

void __sch_recy();

//第一次启动的协程
void __sch_start(__Context *ctx){

    //设置标识位
    ctx->flag |= 1;

    //计算栈底，保证栈16b对齐
    u64 rbp;
    get_reg(rbp, rbp);
    u64 *rsp = __S_zerqaq.routinue_s_base + 0x8 + (rbp & 0xfff) + 8;
    if (ctx->arg_num > 6) rsp += (ctx->arg_num - 6) * 8;

    //保存栈底
    ctx->rbp = rsp;
    printf("in sch_start: bef_p_arg_rsp:%llx\n", rsp);

    //恢复参数上下文，六个以上的参数放在栈中
    if(ctx->arg_num > 6){
        for(int i = ctx->arg_num - 1; i >= 6; i--){
            *(--rsp) = ctx->args[i];
        }
    }
    *(--rsp) = __sch_recy;

    printf("in sch_start: aft_p_arg_rsp:%llx\n", rsp);

#define __ck_st_arg_asm(imm, next_imm, ind, reg)\
    "__CEK_ARG" #imm ": cmpq $" #imm ", %%rax\n\t"\
    "jae __SET_ARG" #imm "\n\t"\
    "jmp __SET_ARG_END\n\t"\
    "__SET_ARG" #imm ": movq " #ind "(%1), %%" #reg "\n\t"\
    "jmp __CEK_ARG" #next_imm "\n\t"

// 其余的6个参数分别放在 rdi rsi rdx rcx r8 r9
    //set_reg(rsp, rsp);
    __asm__ __volatile__ (
        __ck_st_arg_asm(1, 2, 0, rdi)
        __ck_st_arg_asm(2, 3, 8, rsi)
        __ck_st_arg_asm(3, 4, 16, rdx)
        __ck_st_arg_asm(4, 5, 24, rcx)
        __ck_st_arg_asm(5, 6, 32, r8)
        __ck_st_arg_asm(6, 7, 40, r9)
        "__CEK_ARG7:\n\t"
        "__SET_ARG_END:\n\t"
        //jmp到函数地址
        "movq %3, %%rsp\n\t"
        "push %2\n\t"
        "retq\n\t"
        :
        :"a"(ctx->arg_num), "b"(ctx->args), "r"(ctx->addr), "r"(rsp)
        :"rdi", "rsi", "rdx", "rcx", "r8", "r9"
    );
}

#define __jmp_to_sch\
    __asm__ __volatile__(\
        "pushq %0\n\t"\
        "ret"\
        ::"r"(__sch_scheduler)\
    );

void __sch_recover(__Context *ctx);

void __sch_scheduler(){
    //printf("%d\n", list_is_empty(__S_zerqaq.ready));
    //检查链表是否为空
    /* if (list_is_empty(__S_zerqaq.ready)){
        if(list_is_empty(__S_zerqaq.waiting)){
            printf("program end.\n");
        } else{
            printf("dead lock!\n");
        }
        __EXIT;
    } */

    //取ready队列中的第一个协程运行
    __ListNode *node = __S_zerqaq.ready->next;
    if(node->val != 1) {printf("sch:bad node memory\n"); puts(""); __EXIT}

    __Context *ctx = __S_zerqaq.routinue_ctxs[node->val];
    if(ctx->id != 1) {printf("sch:bad ctx memory\n"); puts(""); __EXIT}
    
    __S_zerqaq.running = node->val;
    //list_del(node);
    //printf("%d\n", list_is_empty(__S_zerqaq.ready));
    puts("sch: recover or start jmp");
    if (ctx->flag & 1) __sch_recover(ctx);
    else __sch_start(ctx);
}

//保存协程上下文
void __sch_save_ctx(){
    //保存栈
    __Context *ctx = __S_zerqaq.routinue_ctxs[__S_zerqaq.running];
    u8 *rsp; get_reg(rsp, rsp);
    u64 stack_size = ctx->rbp - rsp;

    puts("in s_sctx:");
    printf("rsp:%p ctx->rbp:%p size: %llx\n", rsp, ctx->rbp, stack_size);
    if(ctx->stack_size < stack_size){
        if(ctx->stack_size != 0) free(ctx->stack);
        puts("s_sctx:mallocing");
        ctx->stack = malloc(stack_size);
        if(ctx->stack == 0) perror("malloc error");
    }
    puts("s_sctx:copying stack");

    for(int i = 0; i < stack_size; i++){
        //printf("%x ", *(rsp + i));
        ctx->stack[i] = rsp[i];
    } //putchar('\n'); 

    //memcpy(ctx->stack, rsp, ctx->stack_size);

    for(int i = 0; i < stack_size; i++) {
        printf("%x ", ctx->stack[i]);
    }
    puts("");

    //把协程切换到到调度器的等待队列 TODO:改成waiting
    //__ListNode *node = ctx->node_ptr;
    //list_add(__S_zerqaq.ready, node);

    //保存指向return的地址
    __asm__ __volatile__ (
        "leaq (%%rip), %0\n\t"
        "addq $17, %0\n\t"
        :"=r"(ctx->addr)
    );

    //__EXIT;
    //进入调度器
    printf("save_ctx jmp\n");
    __jmp_to_sch;

    return;
}

//恢复协程上下文 并运行
void __sch_recover(__Context *ctx){
    u8 *rsp = ctx->rbp - ctx->stack_size;
    memcpy(rsp, ctx->stack, ctx->stack_size);
    set_reg(rsp, rsp);
    jmp(ctx->addr);
}

__Context *__ctx;
void __sch_recy(){
    __ctx = __S_zerqaq.routinue_ctxs[__S_zerqaq.running];
    printf("\nroutinue %lld is end.\n", __ctx->id);
    __EXIT
    set_reg(rsp, main_rsp);
    set_reg(rbp, main_rbp);
    __free_context(__ctx);
    __jmp_to_sch;
}