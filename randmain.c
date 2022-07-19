#include "switchrealloc.h"

#define RANDNO 1000

void *malloc_rand(int *p)
{
    double total_time = 0;
    int *n;

    struct timeval start;
    struct timeval end;

    n = malloc(128);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    size_t new_size_of_mem = 128;

    for (int i = 0; i < RANDNO; ++i)
    {
        new_size_of_mem = p[i];
        gettimeofday(&start, NULL);
        n = realloc(n, new_size_of_mem);
        gettimeofday(&end, NULL);
        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    printf("MA:%.0lf\n", total_time);

    free(n);

    return 0;
}

void *mmap_rand(int *p)
{

    double total_time = 0;
    int *n = NULL;

    struct timeval start;
    struct timeval end;
    int init_size = 128;
    int fd = shm_open("/myregion2", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error in shm_open");
        return NULL;
    }

    if (ftruncate(fd, init_size) == -1)
    {
        perror("Error in ftruncate");
        return NULL;
    }

    int *shm_address = mmap(NULL, init_size,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_SHARED, fd, 0);
    if (shm_address == MAP_FAILED)
    {
        perror("Error mmapping the file");
        return NULL;
    }

    /* Increase shard memory */
    for (size_t i = 1; i < RANDNO; ++i)
    {

        /* Does 8 align memory page? */
        int new_size_of_mem = p[i];
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
        shm_address = mremap(shm_address, old_size_of_men, new_size_of_mem, MREMAP_MAYMOVE);
        gettimeofday(&end, NULL);
        if (n == (void *)-1)
        {
            perror("Error on mremap()");
            return NULL;
        }

        shm_address = n;

        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }
    printf("MM:%.0lf\n", total_time);
    munmap(shm_address, p[RANDNO]);

    return 0;
}

void *_malloc_rand(int *p)
{

    double total_time = 0;
    int *n;

    struct timeval start;
    struct timeval end;

    n = _malloc(128);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    for (int i = 0; i < RANDNO; ++i)
    {
        size_t new_size_of_mem = p[i];
        gettimeofday(&start, NULL);
        n = (int *)_realloc(n, new_size_of_mem);
        gettimeofday(&end, NULL);
        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    printf("HY:%.0lf\n", total_time);

    _free(n);

    return 0;
}

int main()
{
    FILE *file = fopen("page.txt", "r");
    int integers[sizeof(int) * RANDNO];

    int i = 0;
    int num;
    while (fscanf(file, "%d", &num) > 0)
    {
        integers[i] = num;
        // printf("N:%d\n", integers[i]);
        i++;
    }
    fclose(file);

    for (int j = 0; j < 5; j++)
    {
        malloc_rand(integers);
        sleep(2);
        // mmap_rand(integers);
        // sleep(2);
        // _malloc_rand(integers);
    }

    

    return 0;
}