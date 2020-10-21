#include "var_arg.h"

#include "stdio.h"

#define __var_sizeof_1(a) (sizeof(a))
#define __var_sizeof_2(a, b) (sizeof(a) + sizeof(b))
#define __var_sizeof_3(a, b, c) (sizeof(a) + sizeof(b) + sizeof(c))

#define var_sizeof(...) var_args_func_reload(__var_sizeof, ##__VA_ARGS__)

int main(){
    double x;
    int y;
    char z;
    int size1 = var_sizeof(x, y, z);
    int size2 = var_sizeof(x, y);
    int size3 = var_sizeof(x);
    int size4 = var_sizeof(1UL, 2.0);
    printf("%d %d %d %d\n", size1, size2, size3, size4);
    //output: 13 12 8 12
}
