#ifndef CHANNEL_H

#define CHANNEL_H

#include "var_arg.h"
#include "scheduler.h"

//#include "type.h"
#include <stdio.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct{
    u64 data_size, len, cap, head, tail;
    u64 flag;
    u8 *data;
} __Channel;

typedef __Channel* Channel;

void __EXIT();

__Channel* __channel_init(u64 num, u64 data_size, ...);

void __channel_block_read(__Channel *ch);

void __channel_block_write(__Channel *ch);

int __channel_w_read_not_empty(__Channel *ch);

void __channel_await_read(__Channel *ch);

int __channel_w_write_not_empty(__Channel *ch);

void __channel_await_write(__Channel *ch);

void __channel_free(__Channel *ch);

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
    if(sizeof(src) != (c)->data_size) {_panic("Channel: you are tring to write a wrong type to channel."); }\
    *((typeof(src)*)((c)->data) + (c)->tail) = src;\
    (c)->len++;\
    if(++(c)->tail == (c)->cap) (c)->tail = 0;\
    if(__channel_w_read_not_empty(c)) __channel_await_read(c);

//只能在channel不为空的时候使用
#define __channel_read(c, des)\
    if(sizeof(des) != (c)->data_size) {_panic("Channel: you are tring use a wrong type to read from channel."); }\
    memcpy(&des, ((c)->data + (c)->head * (c)->data_size), sizeof(des));\
    (c)->len--;\
    if(++(c)->head == (c)->cap) c->head = 0;\
    if(__channel_w_write_not_empty(c)) __channel_await_write(c);

#define chwrite(c, src)\
    {\
        if((((c)->flag) & 2) == 0) {_panic("Channel: you are trying to read an uninitialize channel\n"); }\
        if(!chok(c)) {_panic("Channel: you are trying to write to a closed channel\n"); }\
        if(__channel_is_full(c)) {\
            while(__channel_is_full(c)){\
                __channel_block_write(c);\
            }\
            __channel_write(c, src);\
        } else {\
            __channel_write(c, src);\
        }\
    }

#define chread(c, des)\
    {\
        if((((c)->flag) & 2) == 0) {_panic("Channel: you are trying to read an uninitialize channel");}\
        if(chok(c)){\
            if(__channel_is_empty(c)){\
                while(__channel_is_empty(c)){\
                    __channel_block_read(c);\
                }\
                __channel_read((c), des);\
            } else {\
                __channel_read((c), des);\
            }\
        } else {_panic("Channel: you are trying to read a closed channel");}\
    }

#define chclose(c)\
    ((c)->flag |= 1)

#define chok(c)\
    (((c)->flag & 1) == 0)

#define chfree(c)\
    __channel_free(c)

#endif