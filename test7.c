#include <stdlib.h>
#include <stdio.h>

#include "type.h"
#include "asm_marco.h"

#define concat(a, b) a ## b

#define _asm(a, b)\
        "mov $" #a ", %%rax\n\t"\
        "mov $"\
        #b\
        ", %%rbx\n\t"\
        "cmpq %%rax, %%rbx\n\t"\
        "jae __JUMP\n\t"\
        "mov $1, %0\n\t"\
        "jmp __JUMP2\n\t"\
        "__JUMP:\n\t"\
        "mov $2, %0\n\t"\
        "__JUMP2:"\

#define asm(a, b) _asm(concat($, a), concat($, b))

#define PRINT_MACRO_HELPER(x) #x
#define PRINT_MACRO(x) #x"="PRINT_MACRO_HELPER(x)

#define TEST 100

#pragma message( PRINT_MACRO( _asm(1, 2) ) )

u64 __exit_addr;

#include "list.h"

void L_add(__ListNode *head, u64 val){
    __ListNode *n = malloc(sizeof(__ListNode));
    n->val = val;
    list_add(head, n);
}

void L_prt(__ListNode *head){
    for(__ListNode *t = head->next; t != head; t = t->next){
        printf("%lld ", t->val);
    }
    printf("\n");
}

void test(){
    u8 *rbp;
    get_reg(rbp, rbp);
    printf("%c", rbp[10]);
}

int main(){
    /* u64 x;
    __asm__ __volatile__ (
        _asm(12, 11)
        :"=r"(x)
    );
    printf("%lld\n", x);
    get_rip(__exit_addr);
    exit(0); */

    __ListNode *h;
    list_init(h);
    
    L_add(h, 1);

    list_del(h->next);

    for(int i = 0; i < 10; i++){
        L_add(h, i);
    }
    L_prt(h);
}