#include "src/Zroutinue.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct rectangle{
    double width, length;
} Rect;

//生产者，生产10个随机矩形放入channel中
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

//消费者，不断从channel中取出矩形，并计算他的面积
void consumer(Channel ch, Channel end){
    for(;;){
        Rect val;
        //如果ch被关闭
        if(!chok(ch)){
            //向end写入一个1， 结束函数
            chwrite(end, 1);    
            return;
        }
        chread(ch, val);
        printf("%.4lf\n", val.width * val.length);
    }
}

void entry(){
    srand(clock());

    //mkch是channel创建函数(make_channel)
    //mkch(数据类型, channel缓冲区大小) 第二个参数可选，默认值为1 
    Channel ch = mkch(Rect), end = mkch(int);

    //go可以开启一个协程
    //go(函数名, 函数参数1, 函数参数2, ...)
    go(producer, ch);
    go(consumer, ch, end);

    //当第一个协程被结束的时候，程序会自动结束，为了让produceer和consumer能够运行，enter阻塞在chread
    int temp;
    chread(end, temp);

    //释放channel
    chfree(end);
    chfree(ch);

    return;
}
int main(){
    //你可以设置协程的栈大小，默认值是1MB
    set_routinue_stack_size(1 << 16); //设置为64KB
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