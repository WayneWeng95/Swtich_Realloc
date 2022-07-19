#include "switchrealloc.h"

int main(){

    int *p;

    p = (int*) _malloc(1024);

    p[0] = 0; 

    int *pp = (int*) _realloc(p,2048);

    pp[1] = 1;

    int *temp = _realloc(pp,1024*1024); 

    temp[2] = 2;

    temp[1024*128] = 999;

    int *temp2 = _realloc(temp,1024*4096);

    temp2[3] = 3;

    temp2[(1024*2048)/8-100] = (1024*2048)/8-100;


    printf("temp2[0]is:%d\n",temp2[0]);
    printf("temp2[1]is:%d\n",temp2[1]);
    printf("temp2[2]is:%d\n",temp2[2]);
    printf("temp2[3]is:%d\n",temp2[3]);
    printf("temp2[1024*128]is:%d\n",temp2[1024*128]);
    printf("temp2[a lot]is:%d\n",temp2[(1024*2048)/8-100]);

    //     n[112324] = 2077;
    // printf("the data inside is %d\n", n[112324]);

    // printf("size of int is: %ld\n",sizeof(int));
    // printf("size of char is: %ld\n",sizeof(char));
    // printf("size of the header pointer is %ld\n",sizeof(switch_alloc_header*));

    _free(temp2);

}