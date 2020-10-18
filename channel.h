#include "type.h"
#include "var_arg.h"
#include "list.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* typedef struct{
    u64 data_size, len, cap, head, tail;
    u8 *data;
} __Channel; */

#ifndef CHANnEL_H

#define CHANnEL_H

__Channel channel_init(u64 num, u64 data_size, ...){
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
    __Channel ret;
    ret.data_size = data_size;
    ret.cap = cap;
    ret.len = ret.head = ret.tail = 0;
    ret.data = (u8*) malloc(cap * data_size);
    list_init(ret.waiting);

    return ret;
}

#define mkch(type, ...) var_args_func(channel_init, sizeof(type), ##__VA_ARGS__)

#define channel_is_empty(c)\
    (c.len == 0)

#define channel_is_not_empty(c)\
    (c.len > 0)

#define channel_is_full(c)\
    (c.len == c.cap)

#define channel_is_not_full(c)\
    (c.len < c.cap)

#define channel_writable(c)\
    channel_is_not_full(c)

#define channel_readable(c)\
    channel_is_not_empty(c)

//只能在channel不满的时候使用
#define channel_write(c, src)\
    *((typeof(src)*)c.data + c.tail) = src;\
    c.len++;\
    if(++c.tail == c.cap) c.tail = 0;

//只能在channel不为空的时候使用
#define channel_read(c, des)\
    memcpy(&des, (c.data + c.head * c.data_size), sizeof(des));\
    c.len--;\
    if(++c.head == c.cap) c.head = 0;

#endif