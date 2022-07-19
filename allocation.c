
#include "switchrealloc.h"
#include "Comparison.h"
#define RANDNO 1000

void *mmap_alloc(int num)
{
    size_t size_of_mem = PAGE_ALIGN(num);

    start_time = my_clock();

    int fd = shm_open("/myregion", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR); // permision specified
    if (fd == -1)
    {
        perror("Error in shm_open");
        return NULL;
    }

    if (ftruncate(fd, size_of_mem) == -1)
    {
        perror("Error in ftruncate");
        return NULL;
    }

    void *shm_address = mmap(NULL, size_of_mem, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
    if (shm_address == MAP_FAILED)
    {
        perror("Error mmapping the file");
        return NULL;
    }

    memcpy(shm_address, buffer, num);

    end_time = my_clock();

    // pid_t tid = gettid();
    // // printf("MM : %d,%.0lf\n", tid, total_time);
    printf("%.0lf\n", end_time - start_time);
    munmap(shm_address, num);

    return 0;
}

void *malloc_alloc(int num)
{

    int *n;
    start_time = my_clock();
    n = malloc(num);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    memcpy(n, buffer, num);

    end_time = my_clock();

    // sleep(30);

    // printf("MA : %d,%.0lf\n", tid, total_time);
    printf("%.0lf\n", end_time - start_time);

    free(n);

    return 0;
}

void *_malloc_alloc(int num)
{

    int *n;

    start_time = my_clock();

    n = _malloc(num);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }
    memcpy(n, buffer, num);

    end_time = my_clock();

    // printf("HY:%.0lf\n", end_time - start_time);
    printf("%.0lf\n", end_time - start_time);

    _free(n);

    return 0;
}

int main()
{

    pFile = fopen(filePath, "r");
    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    rewind(pFile);
    buffer = (char *)malloc(sizeof(char) * fSize);
    testBuff = fread(buffer, 1, fSize, pFile);

    int num = 1073741824;

    malloc_alloc(num);
    sleep(5);
    //_malloc_alloc(num);
    // sleep(2);
    // mmap_alloc(num);

    return 0;
}