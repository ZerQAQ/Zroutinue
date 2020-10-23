#include<stdio.h>
int main(){
    printf("?");
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        printf("yes");
    #endif
}