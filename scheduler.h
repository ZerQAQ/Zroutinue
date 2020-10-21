#ifndef __Z_SCHEDULER_H

#define __Z_SCHEDULER_H

//usage: go(func_name, func_arg1, func_arg2, ...)
#define go(func_name, ...) var_args_func(__go_raw, func_name, #func_name, ##__VA_ARGS__)

//usage: Zroutinue_init(func_name, func_arg1, func_arg2, ...)
#define Zroutinue_init(...)\
    __asm__ __volatile__(\
        "movq %%rbp, %0\n\t"\
        "movq %%rsp, %1\n\t"\
        :"=m"(__main_rbp), "=m"(__main_rsp)\
    );\
    __sch_init();\
    go(__VA_ARGS__);\
    __sch_scheduler();\

#define _panic(msg) __panic(msg, __FILE__, __LINE__)

extern unsigned long long __main_rsp, __main_rbp;

extern void set_routinue_stack_size(unsigned long long);

extern void __sch_save_ctx();

extern void __go_raw(unsigned long long, void*, const char*, ...);

extern void __sch_scheduler();

extern void __sch_init();

void __panic(const char*, const char*, unsigned long long);

extern void __EXIT();

#endif