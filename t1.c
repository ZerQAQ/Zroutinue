#include "sys/sysinfo.h"
#include "stdio.h"
#include "src/type.h"
#include "pthread.h"

typedef u64 mutex;

mutex M = 0;
u64 t_num = 0;
static __thread u64 t_id;

u64 get_lock(mutex *m){
    u64 res = 0;
    __asm__ __volatile(
        "movq $1, %%rbx\n\t"
        "LOCK cmpxchg %%rbx, %1\n\t"
        :"+a"(res), "=m"(*m)
    );
    return !res;
}

u64 free_lock(mutex *m){
    *m = 1;
}

int main(){
    printf("system cpu num is %d\n", get_nprocs_conf());
    printf("system enable num is %d\n", get_nprocs());
    printf("%lld\n", get_lock(&M));
    printf("%lld\n", get_lock(&M));
}