#include "sys/sysinfo.h"
#include "stdio.h"

int main(){
    printf("system cpu num is %d\n", get_nprocs_conf());
    printf("system enable num is %d\n", get_nprocs());
}