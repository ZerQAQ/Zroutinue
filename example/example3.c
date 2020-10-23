#include "Zroutinue.h"

void f1(Channel ch){
    int temp;
    chread(ch, temp);
}

void f2(Channel ch){
    int temp;
    chread(ch, temp);
}

/*
所有的协程都将被阻塞，最后会输出:
dead lock!
these routinues sleep forever:
rouintue 1(entry)
rouintue 2(f1)
rouintue 3(f1)
rouintue 4(f2)
*/
void entry(){
    Channel ch = mkch(int);
    go(f1, ch);
    go(f1, ch);
    go(f2, ch);
    int temp;
    chread(ch, temp);
}

int main(){
    Zroutinue_init(entry);
}