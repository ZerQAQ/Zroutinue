#ifndef __LIST_H
#define __LIST_H

#include "type.h"
#include <stdlib.h>

#define list_init(o)\
    o = (__ListNode*) malloc(sizeof(__ListNode));\
    o->next = o->prev = o;

//  (o->prev)<->(o)
// (o->prev)<->node<->(o)

//添加到列表中
void list_add(__ListNode *head, __ListNode *node);

//从列表中删除 不free
void list_del(__ListNode *node);

void list_del_free(__ListNode *node);

#define list_head(o)\
    (o->next)

#define list_is_empty(head)\
    (head->next == head)

#define list_node_new(_val, var_name)\
    __ListNode *var_name = malloc(sizeof(__ListNode));\
    var_name->val = (void*)_val;

#endif