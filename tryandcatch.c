#include "switchrealloc.h"

int main()
{

    int *i1;
    int *i2;
    int *i3;

    // char *c1;
    // char *c2;
    // char *c3;

    i1 = (int *)_malloc(1024);
    i2 = (int *)_malloc(5000);
    i3 = (int *)_malloc(1024 * 1024);

    // c1 = (char *)_malloc(1024);
    // c2 = (char *)_malloc(8096);
    // c3 = (char *)_malloc(1024 * 1024);

    i1[0] = 0;
    i2[0] = 1;
    i3[0] = 2;

    // c1[0] = 'a';
    // c2[0] = 'b';
    // c3[0] = 'c';

    int *i1_1 = (int *)_realloc(i1, 2048);
    int *i2_1 = (int *)_realloc(i2, 2048);
    int *i3_1 = (int *)_realloc(i3, 2048);

    i1_1[50] = 10;
    i2_1[50] = 11;
    i3_1[50] = 12;

    printf("i1[0]is: %d\n", i1_1[0]);
    printf("i2[0]is: %d\n", i2_1[0]);
    printf("i3[0]is: %d\n", i3_1[0]);

    printf("i1[50]is: %d\n", i1_1[50]);
    printf("i2[50]is: %d\n", i2_1[50]);
    printf("i3[50]is: %d\n", i3_1[50]);

    int *i1_2 = (int *)_realloc(i1_1, 8096);
    int *i2_2 = (int *)_realloc(i2_1, 8096);
    int *i3_2 = (int *)_realloc(i3_1, 8096);

    i1_2[2000] = 20;
    i2_2[2000] = 21;
    i3_2[2000] = 22;

    printf("i1[2000]is: %d\n", i1_2[2000]);
    printf("i2[2000]is: %d\n", i2_2[2000]);
    printf("i3[2000]is: %d\n", i3_2[2000]);

    _free(i1_2);
    _free(i2_2);
    _free(i3_2);

    // int i1_1 = (int*)_realloc(i1,2048);
    // int i1_1 = (int*)_realloc(i1,2048);
    // int i1_1 = (int*)_realloc(i1,2048);

    // int *p;

    // p = (int *)_malloc(1024);

    // p[0] = 0;

    // printf("p[0]is:%d\n", p[0]);

    // int *pp = (int *)_realloc(p, 2048);

    // pp[1] = 1;

    // printf("pp[1]is:%d\n", pp[1]);

    // int *temp = _realloc(pp, 1024 * 8); // failed part       from 2048 grow to 1024K so check that control this should go mmap area (could be mset)

    // temp[2] = 2;

    // temp[1024] = 999;

    // printf("temp[0]is:%d\n", temp[0]);
    // printf("temp[1]is:%d\n", temp[1]);
    // printf("temp[2]is:%d\n", temp[2]);
    // printf("temp[1024]is:%d\n", temp[1024]);

    // int *temp2 = _realloc(temp, 1024 * 4096);

    // temp2[3] = 3;

    // temp2[(1024 * 2048) / 8 - 100] = (1024 * 2048) / 8 - 100;

    // // printf("size check %ld\n",check_size(temp2));

    // printf("temp2[0]is:%d\n", temp2[0]);
    // printf("temp2[1]is:%d\n", temp2[1]);
    // printf("temp2[2]is:%d\n", temp2[2]);
    // printf("temp2[3]is:%d\n", temp2[3]);
    // printf("temp2[1024]is:%d\n", temp2[1024]);
    // printf("temp2[a lot]is:%d\n", temp2[(1024 * 2048) / 8 - 100]);

    // _free(temp2);
}
