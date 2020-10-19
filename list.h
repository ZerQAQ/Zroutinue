#ifndef __LIST_H
#define __LIST_H

#include "type.h"

#define list_init(o)\
    o = (__ListNode*) malloc(sizeof(__ListNode));\
    o->next = o->prev = o;

//  (o->prev)<->(o)
// (o->prev)<->node<->(o)

//添加到列表中
void list_add(__ListNode *head, __ListNode *node){
    __ListNode *p = head->prev;
    p->next = node;
    node->prev = p;
    node->next = head;
    head->prev = node;
}

//从列表中删除 不free
void list_del(__ListNode *node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

#define list_head(o)\
    (o->next)

#define list_is_empty(head)\
    (head->next == head)

#endif