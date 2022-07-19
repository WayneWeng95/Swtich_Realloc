#include "Comparison.h"
#include "switchrealloc.h"


void *mmap_thread()
{
    size_t size_of_mem = INIT_SIZE;

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

    /* Increase shard memory */
    size_t new_size_of_mem = 4096;
    for (size_t i = 0; i < looping; ++i)
    {

        /* Does 8 align memory page? */
        new_size_of_mem = new_size_of_mem * 2;

        /*
           mremap() works with  aligned memory pages.
           How to properly increase shared memory in this case?
        */
        // gettimeofday(&start, NULL);
        start_time = my_clock();
        if (ftruncate(fd, new_size_of_mem) == -1)
        {
            perror("Error in ftruncate");
            return NULL;
        }
        shm_address = (char *)mremap(shm_address, new_size_of_mem / 2, new_size_of_mem, MREMAP_MAYMOVE);
        end_time = my_clock();
        memcpy(shm_address, buffer, new_size_of_mem);
        // char *output = (char *)shm_address;
        // printf("%c\n",output[new_size_of_mem/8]);
        sleep(SLEEP_TIME);
        if (shm_address == (void *)-1)
        {
            perror("Error on mremap()");
            return NULL;
        }

        total_time += end_time-start_time;
    }
    //pid_t tid = gettid();
    // printf("MM : %d,%.0lf\n", tid, total_time);
    printf("%.0lf,",total_time);
    munmap(shm_address, new_size_of_mem);

    return 0;
}

// void *mmaprelease_thread()
// {
//     size_t size_of_mem = INIT_SIZE;
//     int fd = shm_open("/myregion2", O_CREAT | O_RDWR,
//                       S_IRWXO | S_IRUSR | S_IWUSR);
//     if (fd == -1)
//     {
//         perror("Error in shm_open");
//         return NULL;
//     }

//     if (ftruncate(fd, size_of_mem) == -1)
//     {
//         perror("Error in ftruncate");
//         return NULL;
//     }

//     int *shm_address = mmap(NULL, size_of_mem,
//                             PROT_READ | PROT_WRITE | PROT_EXEC,
//                             MAP_SHARED, fd, 0);
//     if (shm_address == MAP_FAILED)
//     {
//         perror("Error mmapping the file");
//         return NULL;
//     }

//     shm_address[0] = 1024;

//     /* Increase shard memory */
//     size_t new_size_of_mem = INIT_SIZE;
//     for (size_t i = 0; i < looping; ++i)
//     {

//         /* Does 8 align memory page? */
//         new_size_of_mem = new_size_of_mem * 2;

//         gettimeofday(&start, NULL);
//         if (ftruncate(fd, new_size_of_mem) == -1)
//         {
//             perror("Error in ftruncate");
//             return NULL;
//         }
//         n = mmap(NULL, new_size_of_mem,
//                  PROT_READ | PROT_WRITE | PROT_EXEC,
//                  MAP_SHARED, fd, 0);
//         memcpy(n, shm_address, new_size_of_mem / 2);
//         //munmap(n,new_size_of_mem/2);
//         //n = mremap(shm_address, new_size_of_mem / 2, new_size_of_mem, MREMAP_MAYMOVE);
//         gettimeofday(&end, NULL);
//         shm_address = n;
//         // sleep(1);
//         n[new_size_of_mem / 8 - 100] = 1;
//         // printf("the data inside is %d\n", n[new_size_of_mem / 8 - 100]);
//         if (n == (void *)-1)
//         {
//             perror("Error on mremap()");
//             return NULL;
//         }

//         //shm_address = n;

//         size_of_mem = new_size_of_mem;

//         total_time += 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
//     }
//     pid_t tid = gettid();
//     printf("MM release : %d,%.0lf\n", tid, total_time);
//     munmap(shm_address, new_size_of_mem);

//     return 0;
// }

void *malloc_thread()
{

    n = NULL;

    n = (char *)malloc(PAGE_SIZE);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    size_t new_size_of_mem = INIT_SIZE;

    for (int i = 0; i < looping; ++i)
    {
        new_size_of_mem = new_size_of_mem * 2;
        start_time = my_clock();
        n = realloc(n, new_size_of_mem);
        end_time = my_clock();
        memcpy(n, buffer, new_size_of_mem);
        sleep(SLEEP_TIME);
        total_time += end_time-start_time;
    }

    //pid_t tid = gettid();
    
    sleep(30);

    // printf("MA : %d,%.0lf\n", tid, total_time);
    printf("%.0lf,",total_time);

    free(n);

    return 0;
}

void *_malloc_thread() // This is a thread for simulating the function
{
    n = _malloc(PAGE_SIZE);

    if (n == NULL)
    {
        printf("Error in malloc!");
        return NULL;
    }

    size_t new_size_of_mem = INIT_SIZE - SWITCH_OFFSET;

    for (int i = 0; i < looping; ++i)
    {
        new_size_of_mem = new_size_of_mem * 2;
        start_time = my_clock();
        n = (char *)_realloc(n, new_size_of_mem);
        end_time = my_clock();
        memcpy(n, buffer, new_size_of_mem);
        sleep(SLEEP_TIME);
        total_time += end_time-start_time;
    }

    //pid_t tid = gettid();

    // printf("HY : %d,%.0lf\n", tid, total_time);
    printf("%.0lf,",total_time);

    _free(n);

    return 0;
}