#include "Comparision.h"
#include "switchrealloc.c"

void *mmap_thread()
{
    size_t size_of_mem = 4096;
    int fd = shm_open("/myregion2", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR);
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

    int *shm_address = mmap(NULL, size_of_mem,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_SHARED, fd, 0);
    if (shm_address == MAP_FAILED)
    {
        perror("Error mmapping the file");
        return NULL;
    }

    shm_address[0] = 1024;

    /* Increase shard memory */
    size_t new_size_of_mem = 4096;
    for (size_t i = 0; i < looping; ++i)
    {

        /* Does 8 align memory page? */
        new_size_of_mem = new_size_of_mem * 2;

        if (ftruncate(fd, new_size_of_mem) == -1)
        {
            perror("Error in ftruncate");
            return NULL;
        }

        /*
           mremap() works with  aligned memory pages.
           How to properly increase shared memory in this case?
        */
        gettimeofday(&start, NULL);
        int *n = mremap(shm_address, new_size_of_mem / 2, new_size_of_mem, MREMAP_MAYMOVE);
        gettimeofday(&end, NULL);
        // sleep(1);
        // n[new_size_of_mem / 8 - 100] = new_size_of_mem / 8 - 100; 
        // printf("the data inside is %d\n", n[new_size_of_mem / 8 - 100]);
        if (n == (void *)-1)
        {
            perror("Error on mremap()");
            return NULL;
        }

        shm_address = n;

        size_of_mem = new_size_of_mem;

        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }
    pid_t tid = gettid();
    printf("MM : %d,%.0lf\n", tid, total_time);
    munmap(shm_address, new_size_of_mem);

    return 0;
}

void *malloc_thread()
{

    int *n = NULL;

    n = malloc(PAGE_SIZE);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    n[0] = 1024;

    size_t new_size_of_mem = 4096;

    for (int i = 0; i < looping; ++i)
    {
        new_size_of_mem = new_size_of_mem * 2;
        gettimeofday(&start, NULL);
        n = realloc(n, new_size_of_mem);
        gettimeofday(&end, NULL);
        // n[new_size_of_mem / 8 - 100] = new_size_of_mem / 8 - 100;
        // printf("the data inside is %d\n", n[new_size_of_mem / 8 - 100]);
        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    pid_t tid = gettid();

    printf("MA : %d,%.0lf\n", tid, total_time);

    free(n);

    return 0;
}

void *switch_thread() //This is a thread for simulating the function
{

    int *n = NULL;

    n = switch_malloc(PAGE_SIZE);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    n[0] = 1024;

    size_t new_size_of_mem = 4096;

    for (int i = 0; i < looping; ++i)
    {
        new_size_of_mem = new_size_of_mem * 2;
        gettimeofday(&start, NULL);
        n = (int*)switch_realloc(n, new_size_of_mem);
        gettimeofday(&end, NULL);
        // sleep(1);
        // n[new_size_of_mem / 8 - 100] = new_size_of_mem / 8 - 100;
        // printf("the data inside is %d\n", n[new_size_of_mem / 8 - 100]);
        total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    pid_t tid = gettid();

    printf("Hybird : %d,%.0lf\n", tid, total_time);

    switch_free(n);

    return 0;
}