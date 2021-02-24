#include "type.h"
#include "var_arg.h"
#include "list.h"
#include "scheduler.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

extern __Scheduler __S_zerqaq;

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
    ret->flag = 2;
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

int __channel_w_read_not_empty(__Channel *ch){
    return ch->w_read->next != ch->w_read;
}

void __channel_await_read(__Channel *ch){
#ifdef __Z_DEBUG
    __Context *c = (__Context*)(((__ListNode*)ch->w_read->next->val)->val);
    printf("routinue %d(%s) is ready\n", c->id, c->func_name);
#endif
    //取出在读等待队列中第一个节点的数据，这个数据代表一个routinue
    list_del(ch->w_read->next->val);
    //将他转移到调度器中的ready队列
    list_add(__S_zerqaq.ready, ch->w_read->next->val);
    //将读等待队列的第一个节点放入free列表
    __ListNode *t = malloc(sizeof(__ListNode));
    t->val = ch->w_read->next;
    list_add(__S_zerqaq.free_list, t);
    __S_zerqaq.free_list_size ++;
    list_del(ch->w_read->next);
}

int __channel_w_write_not_empty(__Channel *ch){
    return ch->w_write->next != ch->w_write;
}

void __channel_await_write(__Channel *ch){
#ifdef __Z_DEBUG
    __Context *c = (__Context*)(((__ListNode*)ch->w_write->next->val)->val);
    printf("routinue %d(%s) ready\n", c->id, c->func_name);
#endif
    //取出在写等待队列中第一个节点的数据，这个数据代表一个routinue
    list_del(ch->w_write->next->val);
    //将他转移到调度器中的ready队列
    list_add(__S_zerqaq.ready, ch->w_write->next->val);
    //将写等待队列中的第一个节点放入free列表
    __ListNode *t = malloc(sizeof(__ListNode));
    t->val = ch->w_write->next;
    list_add(__S_zerqaq.free_list, t);
    __S_zerqaq.free_list_size ++;

    list_del(ch->w_write->next);
}

void __channel_free(__Channel *ch){
    list_node_new(ch, temp1);
    list_node_new(ch->data, temp2);
    list_add(__S_zerqaq.free_list, temp1);
    list_add(__S_zerqaq.free_list, temp2);
    __S_zerqaq.free_list_size += 2;
}