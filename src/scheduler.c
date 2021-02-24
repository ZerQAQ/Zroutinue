#include "type.h"
#include "list.h"
#include "asm_marco.h"
#include "var_arg.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//#define __Z_DEBUG

u64 __main_rsp, __main_rbp;

__Scheduler __S_zerqaq;

u64 scheduler_stack_size = (1 << 14); //16KB
u64 routinue_stack_size = (1 << 20); //1MB
u64 free_list_max_size = (1 << 10); //1024

void set_routinue_stack_size(u64 val){
    routinue_stack_size = val;
}

void __EXIT(){
    set_reg(rbp, __main_rbp);
    set_reg(rsp, __main_rsp);
    exit(0);
}

void __panic(const char* msg, const char* file, u64 line){
    fprintf(stderr, "\033[1m%s:%d:\033[0m %s\n", file, line, msg);
    __EXIT();
}

void __sch_init(){
    __S_zerqaq.routinue_num = 0;

    list_init(__S_zerqaq.ready);
    if(__S_zerqaq.ready == 0x0) perror("malloc error");
    list_init(__S_zerqaq.waiting);
    if(__S_zerqaq.waiting == 0x0) perror("malloc error");
    list_init(__S_zerqaq.free_list);
    if(__S_zerqaq.free_list == 0x0) perror("malloc error");

    __S_zerqaq.routinue_stack = malloc(routinue_stack_size);
    if(__S_zerqaq.routinue_stack == 0x0) perror("malloc error");
    __S_zerqaq.routinue_s_base = ((u64)(__S_zerqaq.routinue_stack +  routinue_stack_size) & 0xfffffffffffff000UL) - 0x2000;

    __S_zerqaq.sch_stack = malloc(scheduler_stack_size);
    if(__S_zerqaq.sch_stack == 0x0) perror("malloc error");
    __S_zerqaq.sch_s_base = ((u64)(__S_zerqaq.sch_stack +  scheduler_stack_size) & 0xfffffffffffff000UL) - 0x2000;
    u64 rbp; get_reg(rbp, rbp); rbp += 8;
    __S_zerqaq.sch_s_base += rbp & 0xfff;
    __S_zerqaq.free_list_size = 0;
    
#ifdef __Z_DEBUG
    u64 temp1 = (u64)__S_zerqaq.routinue_stack;
    u64 temp2 = temp1 + routinue_stack_size;
    printf("stack is between %llx ~ %llx\n", temp1, temp2);
    temp1 = (u64)__S_zerqaq.sch_stack;
    temp2 = temp1 + scheduler_stack_size;
    printf("sch_stack is between %llx ~ %llx\nsch_base: %p\n", temp1, temp2, __S_zerqaq.sch_s_base);
    puts("");
#endif
}

#define __go(func_name, ...) var_args_func(__go_raw, func_name, #func_name, ##__VA_ARGS__)

void __go_raw(u64 arg_num, void* func_ptr, const char* func_name, ...){
    //新建上下文
    __Context *c = malloc(sizeof(__Context));
    __S_zerqaq.routinue_num++;

    //新建链表节点，放入ready队列
    __ListNode *node = malloc(sizeof(__ListNode));
    node->val = c;
    list_add(__S_zerqaq.ready, node);

    //初始化上下文
    //保存函数指针、函数名、参数
    //初始化标记、调度器节点
    c->node_ptr = node;
    c->id = __S_zerqaq.routinue_num;
    c->addr = func_ptr;
    c->stack_size = 0;
    c->stack = NULL;
    c->flag = 0;
    c->stack_base = NULL;
    c->func_name = func_name;

    c->arg_num = arg_num - 2;
    if(c->arg_num > 0) c->args = malloc(c->arg_num * sizeof(u64));
    else c->args = NULL;

    va_list vl;
    va_start(vl, func_ptr);
    for(int i = 0; i < c->arg_num; i++) c->args[i] = va_arg(vl, u64);
    va_end(vl);

#ifdef __Z_DEBUG
    printf("routinue %d(%s) is asign\n", c->id, c->func_name);
#endif

    //return c;
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
    ctx->stack_base = rsp;

    //恢复参数上下文，六个以上的参数放在栈中
    //如果是WIN64，把参数全放在栈中
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#define reg_arg_num 0
#else
#define reg_arg_num 6
#endif
    if(ctx->arg_num > reg_arg_num){
        for(int i = ctx->arg_num - 1; i >= reg_arg_num; i--){
            *(--rsp) = ctx->args[i];
        }
    }
    *(--rsp) = __sch_recy;


#define __ck_st_arg_asm(imm, ind, reg)\
    "cmpq $" #imm ", %%rax\n\t"\
    "jb __SET_ARG_END\n\t"\
    "movq " #ind "(%1), %%" #reg "\n\t"\

// 其余的6个参数分别放在 rdi rsi rdx rcx r8 r9
// 如果是WIN64 最开头的4个参数分别放在rcx rdx r8 r9
    __asm__ __volatile__ (
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
        __ck_st_arg_asm(1, 0, rcx)
        __ck_st_arg_asm(2, 8, rdx)
        __ck_st_arg_asm(3, 16, r8)
        __ck_st_arg_asm(4, 24, r9)
#else
        __ck_st_arg_asm(1, 0, rdi)
        __ck_st_arg_asm(2, 8, rsi)
        __ck_st_arg_asm(3, 16, rdx)
        __ck_st_arg_asm(4, 24, rcx)
        __ck_st_arg_asm(5, 32, r8)
        __ck_st_arg_asm(6, 40, r9)
#endif
        "__SET_ARG_END:\n\t"
        //jmp到函数地址
        "movq %3, %%rsp\n\t"
        "push %2\n\t"
        "retq\n\t"
        :
        :"a"(ctx->arg_num), "b"(ctx->args), "r"(ctx->addr), "r"(rsp)
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
        :"rcx", "rdx", "r8", "r9"
#else
        :"rdi", "rsi", "rdx", "rcx", "r8", "r9"
#endif
    );
}

#define __jmp_to_sch\
    __asm__ __volatile__(\
        "movq %1, %%rsp\n\t"\
        "pushq %0\n\t"\
        "ret"\
        ::"r"(__sch_scheduler), "r"(__S_zerqaq.sch_s_base)\
    );

void __sch_recover();

typedef struct {
    __Context *ctx;
    u8 *rsp;
    u64 i;
} __sch_recv_ctx;
__sch_recv_ctx __r_ctx;

void __sch_scheduler(){
    //检查free队列
    if (__S_zerqaq.free_list_size > free_list_max_size) __clear_free_list();
    
    //检查链表是否为空
    if (list_is_empty(__S_zerqaq.ready)){
        if(list_is_empty(__S_zerqaq.waiting)){
            printf("program end.\n");
        } else{
            printf("dead lock!\nthese routinues sleep forever:\n");
            for(__ListNode *t = __S_zerqaq.waiting->next; t != __S_zerqaq.waiting; t = t->next){
                __Context *ctx = t->val;
                printf("rouintue %d(%s)\n", ctx->id, ctx->func_name);
            }
        }
        __EXIT();
    }

    //取ready队列中的第一个协程运行
    __ListNode *node = __S_zerqaq.ready->next;
    __Context *ctx = node->val;    
    __S_zerqaq.running = ctx;
    list_del(node);
#ifdef __Z_DEBUG
    printf("routinue %d(%s) run", ctx->id, ctx->func_name); puts("");
#endif
    //flag的第一位表示该routinue是否曾经被运行过
    if (ctx->flag & 1) {
        //不是第一次运行的routinue
        __sch_recover(ctx);
    }
    else {
        //第一次运行的routinue
        __sch_start(ctx);
    }
}

void __sch_save_ctx(){
    __asm__ __volatile__ (
        "movq %%rbx, 32(%0)\n\t"
        "movq %0, %%rbx\n\t"
        "movq %%r12, (%%rbx)\n\t"
        "movq %%r13, 8(%%rbx)\n\t"
        "movq %%r14, 16(%%rbx)\n\t"
        "movq %%r15, 24(%%rbx)\n\t"
        ::"a"(__S_zerqaq.running->reg)
        :"r12", "r13", "r14", "r15", "rbx"
    );
    __Context *ctx = __S_zerqaq.running;
    u64 *rbp;
    get_reg(rbp, rbp);
    ctx->reg[6] = *rbp;
    ctx->reg[5] = (u64)(rbp + 2);
    ctx->addr = *(rbp + 1);

    u64 stack_size = ctx->stack_base - ctx->reg[5];
    stack_size = (stack_size - 1) / 8 + 1;
    if(stack_size > ctx->stack_size){
        if(ctx->stack_size > 0) free(ctx->stack);
        ctx->stack = malloc(stack_size * 8);
    }
    ctx->stack_size = stack_size;
    u8 *rsp = ctx->reg[5];
    __asm__ __volatile__ (
        "rep movsq\n\t"
        ::"c"(ctx->stack_size), "S"(rsp), "D"(ctx->stack)
    );
    //for(int i = 0; i < ctx->stack_size; i++) ((u64*)ctx->stack)[i] = ((u64*)rsp)[i];

    list_add(__S_zerqaq.waiting, ctx->node_ptr);

#ifdef __Z_DEBUG
    printf("routinue %d(%s) is sleep\n", ctx->id, ctx->func_name);
#endif
    __jmp_to_sch;
}

void __sch_recover(__Context *ctx){
    //恢复栈
    u8 *rsp = ctx->reg[5];
    __asm__ __volatile__ (
        "rep movsq\n\t"
        ::"c"(ctx->stack_size), "D"(rsp), "S"(ctx->stack)
    );
    //for(int i = 0; i < ctx->stack_size; i++){((u64*)rsp)[i] = ((u64*)ctx->stack)[i];}
    //恢复寄存器
    __asm__ __volatile__ (
        "movq %0, %%rbx\n\t"
        "movq (%%rbx), %%r12\n\t"
        "movq 8(%%rbx), %%r13\n\t"
        "movq 16(%%rbx), %%r14\n\t"
        "movq 24(%%rbx), %%r15\n\t"
        "movq 40(%%rbx), %%rsp\n\t"
        "movq 48(%%rbx), %%rbp\n\t"
        "movq 32(%0), %%rbx\n\t"
        "pushq %%rdx\n\t"
        //跳转到函数地址
        "retq\n\t"
        ::"a"(ctx->reg), "d"(ctx->addr)
        :"r12", "r13", "r14", "r15", "rbx"
    );
}

__Context *__ctx;
__ListNode *__temp;
//  回收运行结束的协程
void __sch_recy(){
    __ctx = __S_zerqaq.running;
#ifdef __Z_DEBUG
    printf("routinue %lld(%s) is end.\n", __ctx->id, __ctx->func_name);
#endif
    if(__ctx->id == 1) {
#ifdef __Z_DEBUG
        printf("main exit, program end.\n");
#endif
        __EXIT();
    }
    set_reg(rsp, __main_rsp);
    set_reg(rbp, __main_rbp);
    __free_context(__ctx);
    __jmp_to_sch;
}

void __clear_free_list(){
    set_reg(rsp, __main_rsp);
    set_reg(rbp, __main_rbp);
    while(__S_zerqaq.free_list->next != __S_zerqaq.free_list){
        //printf("in free list:");
        __temp = __S_zerqaq.free_list->next;
        list_del(__temp);
        free(__temp->val);
        free(__temp);
    }
    __S_zerqaq.free_list_size = 0;
    __jmp_to_sch;
}