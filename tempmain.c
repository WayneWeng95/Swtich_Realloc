#include "switchrealloc.c"

int main(){

    int *p;

    p = (int*) switch_malloc(1024);

    p[0] = 0; 

    int *temp = switch_realloc(p,1024*1024); 

    temp[1] = 1;

    temp[1024*128] = 999;

    int *temp2 = switch_realloc(temp,1024*4096);

    temp2[2] = 2;

    temp2[(1024*2048)/8-100] = (1024*2048)/8-100;

    printf("temp2[0]is:%d\n",temp2[0]);
    printf("temp2[1]is:%d\n",temp2[1]);
    printf("temp2[2]is:%d\n",temp2[2]);
    printf("temp2[1024*128]is:%d\n",temp2[1024*128]);
    printf("temp2[a lot]is:%d\n",temp2[(1024*2048)/8-100]);

    //     n[112324] = 2077;
    // printf("the data inside is %d\n", n[112324]);

    // printf("size of int is: %ld\n",sizeof(int));
    // printf("size of char is: %ld\n",sizeof(char));
    // printf("size of the header pointer is %ld\n",sizeof(switch_alloc_header*));

    switch_free(temp2);

}