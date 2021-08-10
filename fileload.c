#include "switchrealloc.c"
#include "Comparison.h"

#define INIT 1024
#define RANDNO 500

FILE *pFile;
int fSize, testBuff;
char *buffer;
char *test;
//char filePath[] = "/home/weikangweng/Documents/Thesis/Comparision/data/file.txt"; //change while deploy
//char sizePath[] = "rand7.txt";
int integers[sizeof(int) * RANDNO];

void *malloc_rand(int *p)
{
    char *n;
    n = (char *)malloc(INIT_SIZE);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    for (int i = 0; i < RANDNO; ++i)
    {
        size_t new_size_of_mem = p[i];
        gettimeofday(&start, NULL);
        n = realloc(n, new_size_of_mem);
        gettimeofday(&end, NULL);
        memcpy(n, buffer, new_size_of_mem);
        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    printf("MA:%.0lf\n", total_time);

    free(n);

    return 0;
}

void *mmap_rand(int *p)
{

    //void *n=NULL;

    int init_size = p[0];
    int fd = __create_fd(init_size);

    void *shm_address = mmap(NULL, init_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
    if (shm_address == MAP_FAILED)
    {
        perror("Error mmapping the file");
        return NULL;
    }

    /* Increase shard memory */
    for (size_t i = 1; i < RANDNO; ++i)
    {

        /* Does 8 align memory page? */
        int new_size_of_mem = p[i]; //possible problem point
        int old_size_of_men = p[i - 1];

        /*
           mremap() works with  aligned memory pages.
           How to properly increase shared memory in this case?
        */
        gettimeofday(&start, NULL);
        if (ftruncate(fd, new_size_of_mem) == -1)
        {
            perror("Error in ftruncate");
            return NULL;
        }
        shm_address = (char *)mremap(shm_address, old_size_of_men, new_size_of_mem, MREMAP_MAYMOVE);
        gettimeofday(&end, NULL);
        if (shm_address == (void *)-1)
        {
            perror("Error on mremap()");
            return NULL;
        }
        //printf("%ld\n",i);
        memcpy(shm_address, buffer, new_size_of_mem);

        //shm_address = n;

        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }
    printf("MM:%.0lf\n", total_time);
    munmap(shm_address, p[RANDNO]);

    return 0;
}

void *_malloc_rand(int *p)
{
    char *n;

    n = _malloc(INIT_SIZE);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    for (int i = 0; i < RANDNO; ++i)
    {
        size_t new_size_of_mem = p[i];
        gettimeofday(&start, NULL);
        n = (char *)_realloc(n, new_size_of_mem);
        gettimeofday(&end, NULL);
        memcpy(n, buffer, new_size_of_mem);
        //sleep(1);
        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    printf("HY:%.0lf\n", total_time);

    _free(n);

    return 0;
}

int main(int argc, char *argv[])
{

    FILE *file = fopen(sizePath, "r");

    int mode;

    char *first_para = argv[1];
    mode = atoi(first_para);

    int i = 0;
    int num;
    while (fscanf(file, "%d", &num) > 0)
    {
        integers[i] = num;
        //printf("N:%d\n", integers[i]);
        i++;
    }
    fclose(file);

    pFile = fopen(filePath, "r");
    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    rewind(pFile);
    buffer = (char *)malloc(sizeof(char) * fSize);
    testBuff = fread(buffer, 1, fSize, pFile);

    if (testBuff == fSize)
    {
        switch (mode)
        {
        case 0:
            malloc_rand(integers);
            break;
        case 1:
            mmap_rand(integers);
            break;
        case 2:
            _malloc_rand(integers);
            break;
        default:
            printf("Wrong mode, 0 for malloc, 1 for mmap, 2 for _malloc");
            break;
        }

    }
    fclose(pFile);
    free(buffer);
    //free(test);
}