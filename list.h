#ifndef __LIST_H
#define __LIST_H

#include "type.h"

#define list_init(o)\
    o = (__ListNode*) malloc(sizeof(__ListNode));\
    o->next = o->prev = o;

//  (o->prev)<->(o)
// (o->prev)<->temp<->(o)

#define list_add(o, _val, temp)\
    __ListNode *temp = (__ListNode*) malloc(sizeof(__ListNode));\
    temp->val = _val;\
    o->prev->next = temp;\
    temp->prev = o->prev;\
    temp->next = o;\
    o->prev = temp;

//不能删除头节点
#define list_del(o)\
    o->prev->next = o->next\
    o->next->prev = o->prev;\
    free(o);

#define list_head(o)\
    (o->next)

#endif