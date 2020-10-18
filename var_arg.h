#ifndef VAR_ARG_H

#define VAR_ARG_H

#define __args_num(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))
#define var_args_func(func, ...)  (func(__args_num(__VA_ARGS__), __VA_ARGS__))

#endif