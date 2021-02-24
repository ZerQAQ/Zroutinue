#ifndef __TYPE_H

#define __TYPE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct __list_node {
    void* val;
    struct __list_node *next, *prev;
} __ListNode;

//flag第一位表示是否已经运行过
typedef struct {
    u8 *stack, *stack_base;
    const char *func_name;
    u64 addr, stack_size, flag, id;
    u64 reg[7];
    __ListNode *node_ptr;
    u64 *args, arg_num;
} __Context;

#define __free_context(ctx)\
    if(ctx->stack_size > 0) free(ctx->stack);\
    free(ctx->node_ptr);\
    if(ctx->arg_num > 0) free(ctx->args);\
    free(ctx);

typedef struct{
    u64 routinue_num, routinue_cap, free_list_size;
    __Context *running;
    __ListNode *ready, *waiting, *free_list;

    u8 *routinue_stack, *routinue_s_base;
    u8 *sch_stack, *sch_s_base;
} __Scheduler;

typedef struct{
    u64 data_size, len, cap, head, tail;
    u64 flag;
    u8 *data;
    __ListNode *w_read, *w_write;
} __Channel;

typedef __Channel* Channel;

#endif