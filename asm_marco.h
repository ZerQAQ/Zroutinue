#ifndef __ASM_MARCO_H

#define __ASM_MARCO_H

#define copy_stack(des)\
    __asm__ __volatile__(\
        "movq %%rsp, %%rsi\n\t"\
        "movq %%rbp, %%rcx\n\t"\
        "subq %%rsp, %%rcx\n\t"\
        "rep movsb"\
        ::"D"(des)\
    );

#define get_reg(reg, des)\
    __asm__ __volatile__ (\
        "movq %%"\
        #reg\
        " , %0"\
        :"=r"(des)\
    );

#define get_rsp(des)\
    __asm__ __volatile__(\
        "movq %%rsp, %0"\
        :"=r"(des)\
    );

#define set_reg(reg, val)\
    __asm__ __volatile__(\
        "movq %0, %%"\
        #reg\
        ::"r"(val)\
    );

#define mv_sb_rsp(val)\
    __asm__ __volatile__(\
        "movq %%rbp, %%rsp\n\t"\
        "subq %0, %%rsp"\
        ::"r"(val)\
    );

#define jmp(des)\
    __asm__ __volatile__ (\
        "push %0\n\t"\
        "ret"\
        ::"r"(des)\
    );\

#define label(name)\
    __asm__ __volatile__ (\
        #name\
        ":"\
    );

#endif