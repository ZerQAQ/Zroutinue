#include "type.h"
#include <stdlib.h>

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

void list_del_free(__ListNode *node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
}