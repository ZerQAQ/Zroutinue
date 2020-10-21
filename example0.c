#include "Zroutinue.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct rectangle{
    double width, length;
} Rect;

void producer(Channel ch){
    Rect val;
    for(int i = 0; i < 10; i++){
        val.width = (rand() % 10000) / 100.0;
        val.length = (rand() % 10000) / 100.0;
        chwrite(ch, val);
        printf("%.2lf * %.2lf = ", val.width, val.length);
    }
    chclose(ch);
}

void consumer(Channel ch, Channel end){
    for(;;){
        Rect val;
        if(!chok(ch)){
            chwrite(end, 1);    
            return;
        }
        chread(ch, val);
        printf("%.4lf\n", val.width * val.length);
    }
}

void entry(){
    srand(clock());
    Channel ch = mkch(Rect), end = mkch(int);
    go(producer, ch);
    go(consumer, ch, end);
    int temp;
    chread(end, temp);
    chfree(end);
    chfree(ch);
    return;
}
int main(){
    Zroutinue_init(entry);
}

/*
output:
46.24 * 21.75 = 1005.7200
79.97 * 98.51 = 7877.8447
48.95 * 33.00 = 1615.3500
12.50 * 77.46 = 968.2500
38.92 * 86.57 = 3369.3044
90.76 * 11.19 = 1015.6044
76.04 * 17.96 = 1365.6784
2.74 * 99.60 = 272.9040
73.60 * 92.82 = 6831.5520
98.76 * 15.70 = 1550.5320
*/