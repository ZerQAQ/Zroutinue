#include "type.h"
#include "var_arg.h"
#include "list.h"
#include "scheduler.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* typedef struct{
    u64 data_size, len, cap, head, tail;
    u8 *data;
} __Channel; */

#ifndef CHANNEL_H

#define CHANNEL_H

__Channel* __channel_init(u64 num, u64 data_size, ...){
    //处理参数
    u64 cap;
    if (num == 1){
        cap = 1;
    } else {
        va_list vl;
        va_start(vl, data_size);
        cap = va_arg(vl, int);
        va_end(vl);
    }
    //初始化channel
    __Channel *ret = malloc(sizeof(__Channel));
    ret->data_size = data_size;
    ret->cap = cap;
    ret->len = ret->head = ret->tail = 0;
    ret->data = (u8*) malloc(cap * data_size);
    ret->closed = 0;
    list_init(ret->w_read);
    list_init(ret->w_write);

    return ret;
}

void __channel_block_read(__Channel *ch){
    __ListNode *node = malloc(sizeof(__ListNode));
    node->val = __S_zerqaq.running->node_ptr;
    list_add(ch->w_read, node);
    __sch_save_ctx();
}

void __channel_block_write(__Channel *ch){
    __ListNode *node = malloc(sizeof(__ListNode));
    node->val = __S_zerqaq.running->node_ptr;
    list_add(ch->w_write, node);
    __sch_save_ctx();
}

#define __channel_w_read_not_empty(ch)\
    ((ch)->w_read->next != (ch)->w_read)

void __channel_await_read(__Channel *ch){
#ifdef __Z_DEBUG
    __Context *c = (__Context*)(((__ListNode*)ch->w_read->next->val)->val);
    printf("routinue %d(%s) is ready\n", c->id, c->func_name);
#endif
    list_del(ch->w_read->next->val);
    list_add(__S_zerqaq.ready, ch->w_read->next->val);
    list_del(ch->w_read->next);
}

#define __channel_w_write_not_empty(ch)\
    ((ch)->w_write->next != (ch)->w_write)

void __channel_await_write(__Channel *ch){
#ifdef __Z_DEBUG
    __Context *c = (__Context*)(((__ListNode*)ch->w_write->next->val)->val);
    printf("routinue %d(%s) ready\n", c->id, c->func_name);
#endif
    list_del(ch->w_write->next->val);
    list_add(__S_zerqaq.ready, ch->w_write->next->val);
    list_del(ch->w_write->next);
}

#define mkch(type, ...) var_args_func(__channel_init, sizeof(type), ##__VA_ARGS__)

#define __channel_is_empty(c)\
    ((c)->len == 0)

#define __channel_is_not_empty(c)\
    ((c)->len > 0)

#define __channel_is_full(c)\
    ((c)->len == c->cap)

#define __channel_is_not_full(c)\
    ((c)->len < c->cap)

#define __channel_writable(c)\
    __channel_is_not_full(c)

#define __channel_readable(c)\
    __channel_is_not_empty(c)

#define __channel_blocking(c)\
    (__channel_is_empty(c) || __channel_is_full(c))

//只能在channel不满的时候使用
#define __channel_write(c, src)\
    if(sizeof(src) != (c)->data_size) {printf("Channel: you are tring to write a wrong type to channel.\n"); }\
    *((typeof(src)*)((c)->data) + (c)->tail) = src;\
    (c)->len++;\
    if(++(c)->tail == (c)->cap) (c)->tail = 0;\
    if(__channel_w_read_not_empty(c)) __channel_await_read(c);

//只能在channel不为空的时候使用
#define __channel_read(c, des)\
    if(sizeof(des) != (c)->data_size) {printf("Channel: you are tring use a wrong type to read from channel.\n"); }\
    memcpy(&des, ((c)->data + (c)->head * (c)->data_size), sizeof(des));\
    (c)->len--;\
    if(++(c)->head == (c)->cap) c->head = 0;\
    if(__channel_w_write_not_empty(c)) __channel_await_write(c);

#define chwrite(c, src)\
    {if(__channel_is_full(c)) {\
        while(__channel_is_full(c)){\
            __channel_block_write(c);\
        }\
        __channel_write(c, src);\
    } else {\
        __channel_write(c, src);\
    }}

#define chread(c, des)\
    {if(__channel_is_empty(c)) {\
        while(__channel_is_empty(c)){\
            __channel_block_read(c);\
        }\
        __channel_read(c, des);\
    } else {\
        __channel_read(c, des);\
    }}

#define chclose(c)\
    (c)->close = 1;

#define chok(c)\
    ((c)->close == 0)

#endif