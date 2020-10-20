#include "type.h"
#include "channel.c"
#include "list.h"
#include <stdio.h>

typedef struct {
    int val;
} T;

int main(){
    Channel ch = mkch(T, 10);
    for(int i = 0; i < 5; i++){
        T v; v.val = i;
        channel_write(ch, v);
    }
    for(int i = 5; i < 20; i++){
        T v; v.val = i;
        channel_write(ch, v);
        channel_read(ch, v);
        printf("%d ", v.val);
    }

    /* __ListNode *h;
    list_init(h);

    list_add(h, 1, _1);
    list_add(h, 2, _2);
    list_add(h, 3, _3); */

    /* for(__ListNode *t = h->next; t != h; t = t->next){
        printf("%llu ", t->val);
    } */
}