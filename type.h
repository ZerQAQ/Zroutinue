#ifndef TYPE_H

#define TYPE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct __list_node {
    u64 val;
    __ListNode *next, *prev;
} __ListNode;

typedef struct {
    u8 *stack;
    u64 addr, stack_size, rbp;
} __Context;

typedef struct{
    u64 routinue_num;
    __Context *routinue_ctxs;
    __ListNode *ready, *waiting;
} __Scheduler;

typedef struct{
    u64 data_size, len, cap, ptr;
    u8 *data;
} __Chanel;

#endif