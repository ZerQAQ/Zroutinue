# Zroutinue

一个用C和汇编写的go风格的轻量协程库，包含一个泛型channel，用宏实现了泛型和简单的类型检查，并且支持死锁检测。

目前支持x86-64下的linux、macos和WIN64。

一个简单的例子(example0.c)：
```C
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
```

另一个有趣的例子(example1.c)：
```C
#include "src/Zroutinue.h"

#include <stdio.h>
#include <stdlib.h>

void _generator(Channel ch){
    int val = 2;
    for(;;){
        chwrite(ch, val);
        val++;
    }
}

//返回一个channel, 并开启一个协程往channel中不断传入从2开始的自然数
Channel make_generator(){
    Channel ch = mkch(int);
    go(_generator, ch);
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

//读入一个旧channel，返回一个新channel，并开启一个协程，这个协程从旧channnel得到的数据，过滤掉k的倍数，传入新的channel
Channel make_filter(Channel ch, int k){
    Channel ret = mkch(int);
    go(_filter, ch, ret, k);
    return ret;
}

//打印n个质数
void prim_generator(int n){
    Channel gen = make_generator();
    int prim;
    for(int i = 0; i < n; i++){
        chread(gen, prim);
        printf("%d ", prim);
        gen = make_filter(gen, prim);
    }
    putchar('\n');
}

int main(int argn, char** argv){
    if(argn < 2) {printf("need param, usage: %s <prim_num>\n", argv[0]); return 0;}
    int val = atoi(argv[1]);
    Zroutinue_init(prim_generator, val);
}

/*
output:
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 ...
*/
```

# 如何使用

## 初始化

你需要在main中使用Zroutinue_init来初始化调度器，并提供一个入口函数作为主协程。调度器初始化完毕后将运行它，当主协程结束后，程序将会退出。

```C
void entry(int argn, char **argv){
    ...
}

main(int argn, char **argv){
    Zroutinue_init(entry, argn, argv)
}
```
### go

你可以用go来开启一个协程，目前函数参数只支持channel和基本类型。
```C
go(函数名，函数参数1，函数参数2，...)
```

## channel

### mkch
你可以通过mkch来初始化一个channel，第一个参数是类型，第二个参数是channel的大小，第二个参数是可选的，默认值为1。
```C
Channel ch1 = mkch(int), ch2 = mkch(double, 3);
```

### chfree
channel的本质是一个指针，故使用完毕后需要使用chfree释放:
```C
chfree(ch);
```

### chread、chwrite
你可以通过chread或者chwrite来读写channel
```C
Channel ch = mkch(int);
chwrite(ch, 1);
int temp;
chread(ch, temp);
printf("%d\n", temp);
//输出1
```
当你尝试读写类型大小不符的类型时，会报错并退出程序：
```C
void unsafe(){
    Channel ch = mkch(int, 5);
    chwrite(ch, 1LU);
}
//当unsafe被执行时，输出
//example4.c:10: Channel: you are trying to write a wrong type to channel
```

### chclose
chclose可以用于关闭一个channel，当一个channel被关闭时，读他将不会发生任何事，但尝试写时会报错并退出程序。
```C
void unsafe(){
    Channel ch = mkch(double);
    chclose(ch);
    chwrite(ch, 1.0);
}
//当unsafe被执行时，输出
//example4.c:6: Channel: you are trying to write to a closed channel
```

### chok
你可以通过chok来判断一个ch是否关闭，返回1代表未关闭，返回0代表关闭。
```C
Channel ch = mkch(int);
printf("%d\n", chok(ch)); //1
chclose(ch);
printf("%d\n", chok(ch)); //0
```

# 性能

在我的计算机上完成100万个协程的创建和销毁只使用了0.6秒，测试源码见example2.c
```
$ time ./example2 1000000
1234

real    0m0.669s
user    0m0.375s
sys     0m0.313s
```