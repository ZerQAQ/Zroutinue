#include "scheduler.c"
#include "channel.c"

#define __Z_DEBUG 1

void _generator(Channel ch){
    int val = 2;
    for(;;){
        chwrite(ch, val);
        val++;
    }
}

//返回一个channel channel中不断传入从2开始的自然数
Channel make_generator(){
    Channel ch = mkch(int);
    __go(_generator, ch);
    return ch;
}

void _filter(Channel ch_in, Channel ch_out, int k){
    int val;
    for(;;){
        chread(ch_in, val);
        while(val % k == 0) chread(ch_in, val)
        chwrite(ch_out, val);
    }
}

//读入一个旧channel，返回一个新channel，将旧channnel中得到的数据过滤掉k的倍数之后，传入新的channel
Channel make_filter(Channel ch, int k){
    Channel ret = mkch(int);
    __go(_filter, ch, ret, k);
    return ret;
}

void entry(){
    Channel gen = make_generator();
    int prim;
    for(int i = 0; i < 10; i++){
        chread(gen, prim);
        printf("%d ", prim);
        gen = make_filter(gen, prim);
    }
    putchar('\n');
}

int main(){
    get_reg(rsp, main_rsp);
    get_reg(rbp, main_rbp);

    __sch_init();
    __go(entry);
    // this function will never return
    __sch_scheduler();
}