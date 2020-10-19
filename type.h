#ifndef __TYPE_H

#define __TYPE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct __list_node {
    u64 val;
    struct __list_node *next, *prev;
} __ListNode;

//flag第一位表示是否已经运行过
typedef struct {
    u8 *stack, *stack_base, *rbp;
    u64 addr, stack_size, flag, id;
    __ListNode *node_ptr;
    u64 *args, arg_num;
} __Context;

#define __free_context(ctx)\
    if(ctx->stack_size > 0) free(ctx->stack);\
    free(ctx->node_ptr);\
    if(ctx->arg_num > 0) free(ctx->args);\
    free(ctx);

typedef struct{
    u64 routinue_num, routinue_cap, running;
    __Context **routinue_ctxs;
    __ListNode *ready, *waiting;

    u8 *rbp, *rsp, *routinue_stack, *routinue_s_base;
} __Scheduler;

typedef struct{
    u64 data_size, len, cap, head, tail;
    u8 *data;
    __ListNode *waiting;
} __Channel;

#endif