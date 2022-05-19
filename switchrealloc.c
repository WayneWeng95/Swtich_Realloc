#include "switchrealloc.h"

#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a)-1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)
#define HUGE_PAGE_ALIGN(addr) ALIGN(addr, HUGE_PAGE_SIZE)
#define MID_PAGE_ALIGN(addr) ALIGN(addr, MID_PAGE_SIZE)
#define X2_ALIGN(addr) ALIGN(addr, X2_EH_SIZE)

static void recvSignal(int sig)
{
    siglongjmp(env, 1);
}

static size_t check_size(void *p)
{

    signal(SIGSEGV, recvSignal);
    int r = sigsetjmp(env, 1);
    if (r == 0)
    {
        size_t now_size = malloc_usable_size(p);
        // printf("size : %ld \n", now_size); // stopped here
        return now_size - SMALL_OFFSET;
    }
    else
    {
        int *temp = (int *)p;
        temp = temp - 1;
        int size = *temp;
        // printf("size === %d\n", size);
        if (size <= 0)
        {
            // printf("-size = %d\n", size);
            return -1 * size;
        }
        if (size > 0)
        {
            // printf("size = %d\n", size);
            return size;
        }
        return -1;
    }
}

static void *__go_to_head(void *ptr)
{
    int *plen = (int *)ptr;
    plen = plen - 3;
    return (void *)plen;
}

static int __create_fd(int size)
{

    int fd = shm_open("/myregion", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR); // permision specified
    if (fd == -1)
    {
        perror("Error in shm_open");
        return -1;
    }

    if (ftruncate(fd, size) == -1)
    {
        perror("Error in ftruncate");
        return -1;
    }

    return fd;
}

static void _internal_free_medium(void *ptr)
{

    int *temp;
    temp = (int *)ptr - 1;
    free(temp);
}

static void _internal_free(void *ptr)
{

    int *plen = (int *)__go_to_head(ptr);
    munmap((void *)plen, plen[2]);
}

void *_malloc(size_t size)
{ // In this malloc, switch between glibc malloc(Switch_point) and mmap(Switch_point)

    if (size <= 0)
    {
        perror("Wrong size\n");
        return NULL;
    }
    else if (size <= HEADLESS)
    {
        void *block = NULL;
        block = malloc(size);
        return block;
    }
    else if (size <= MAPPING_POINT)
    {
        size_t len = size + SMALL_OFFSET;
        int *temp;
        temp = (int *)malloc(len); // apply a switch to see whether to mmap/alloc
        if (temp)
        {
            temp[0] = -size;
            return (void *)(&temp[1]);
        }
        else
        {
            perror("Error in malloc\n");
            return NULL;
        }
    }
    else
    {
        size_t len = size + OFFSET;
        int *temp;
        len = PAGE_ALIGN(len);
        int fd = -1;
        fd = __create_fd(len);

        temp = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
        // fprintf(stdout, "Mmap branch\n");
        if (temp == MAP_FAILED)
        {
            perror("Error in mmap\n");
            return NULL;
        }
        else
        {
            temp[0] = fd;   // fd
            temp[1] = 0;    // counter
            temp[2] = size; // size
            return (void *)(&temp[3]);
        }
    }
    return NULL;
}

void *_realloc(void *ptr, size_t size)
{ // once realloc need to allocate space larger than 128Kb it switch from realloc to mremap

    size_t old_size = check_size(ptr);
    // printf("old_size is %ld \n", old_size);
    //   printf("new realloc size is %ld \n", size);

    if (size <= 0)
    {
        perror("Wrong size\n");
    }
    else if (0 <= size && size <= HEADLESS)
    {

        void *block = NULL;
        block = malloc(size);
        // memset(block, 0, size);           //maybe necessary
        if (size >= old_size)
        {
            memcpy(block, ptr, old_size);
        }
        else
        {
            memcpy(block, ptr, size);
        }

        if (old_size <= HEADLESS)
        {
            free(ptr);
        }
        else
        {
            _internal_free_medium(ptr);
        }

        return block;
    }
    else if (size <= MAPPING_POINT && size >= HEADLESS)
    {
        // int *temp = (int *)ptr;
        // temp = temp - 1;

        size_t new_size = size + SMALL_OFFSET;
        if (old_size <= HEADLESS)
        {
            void *block = NULL;
            block = _malloc(size);
            memcpy(block, ptr, old_size);
            free(ptr); // free of the 16 - 128 size area
            return block;
        }
        else if (old_size <= MAPPING_POINT)
        {
            int *temp = (int *)ptr;
            temp = temp - 1;
            // printf("size = %d\n", temp[0]);
            temp = (int *)realloc(temp, new_size);
            if (temp)
            {
                temp[0] = -size;
                return (void *)(&temp[1]);
            }
            else
            {
                perror("Error in malloc\n");
                return NULL;
            }
        }
        else
        {
            new_size = PAGE_ALIGN(new_size);
            int *plen = _malloc(new_size);
            // memset(plen, 0, old_size);       //maybe useful
            memcpy(plen, ptr, size);
            _internal_free(ptr);
            return (void *)(&plen[0]);
        }
    }
    else
    {
        int *temp = (int *)ptr;
        size_t new_size = size + OFFSET;
        // printf("old_size is %ld \n", old_size);
        // printf("new_size is %ld \n", new_size);

        if (old_size <= HEADLESS)
        {
            void *block = NULL;
            block = _malloc(size);
            // memset(temp, 0, old_size);
            memcpy(block, ptr, old_size);
            free(ptr);
            return block;
        }
        else if (old_size <= MAPPING_POINT)
        {
            temp = temp - 1;
            // printf("size = %d\n", temp[0]);
            new_size = PAGE_ALIGN(new_size);

            int *plen = _malloc(new_size);
            // memset(plen, 0, old_size);       //maybe useful
            memcpy(plen, ptr, old_size);
            _internal_free_medium(ptr);
            return (void *)(&plen[0]);
        }
        else
        {
            temp = temp - 3; // move to head;

            int fd = temp[0];
            new_size = PAGE_ALIGN(new_size);

#if ENABLE_HUGLETLB
            if (temp[1] >= MMAP_HOTLEVEL && temp[2] >= HUGE_TLB_POINT)
            {
                new_size = HUGE_PAGE_ALIGN(new_size);
            }
#endif
            if (ftruncate(fd, new_size) == -1)
            {
                perror("Error in ftruncate");
                return NULL;
            }

            temp = (int *)mremap(temp, old_size, new_size, MREMAP_MAYMOVE);

            // This was added in the 5.7 kernel as a new flag to mremap(2) called MREMAP_DONTUNMAP.
            //  This leaves the existing mapping in place after moving the page table entries.
            temp[0] = fd;
            temp[2] = size;
            return (void *)(&temp[3]);
        }
    }

    return NULL;

    //     size_t new_len = size + OFFSET;

    //     int *plen = (int *)ptr;
    //     plen = plen - 3; // move to head;

    //     int fd = plen[0];
    //     size_t old_len = plen[2] + OFFSET;
    //     size_t new_len = size + OFFSET;

    // #if ENABLE_PREDICTION

    // #if ENABLE_X2_ENHANCEMENT
    //     if (size == plen[2] * 2)
    //     {
    //         plen[1] += MMAP_HOTLEVEL / 3;
    //         new_len = X2_ALIGN(new_len);
    //     }
    // #endif

    //     if (old_len > new_len)
    //     {
    // #if ENABLE_UNSHRINK_NOW
    //         if (plen[1] >= UNSHRINK_THRESHOULD)
    //         {
    //             // plen[2] = size;
    //             return (void *)(&plen[3]);
    //         }
    // #endif
    //         plen[1] -= SHRINKING_LEVEL;
    //     }
    //     else
    //     {
    //         plen[1]++;
    //     }

    // #endif

    //     if (new_len <= KB * SWITCH_POINT - AGGRESIVE * PAGE_SIZE)
    //     {
    //         if (plen[0] != -1)
    //         {
    //             new_len = PAGE_ALIGN(new_len);
    //             int *temp;
    //             if (ftruncate(fd, new_len) == -1)
    //             {
    //                 perror("Error in ftruncate");
    //                 return NULL;
    //             }

    //             temp = (int *)mremap(plen, old_len, new_len, MREMAP_MAYMOVE);

    //             // This was added in the 5.7 kernel as a new flag to mremap(2) called MREMAP_DONTUNMAP.
    //             //  This leaves the existing mapping in place after moving the page table entries.
    //             temp[0] = fd;
    //             temp[2] = size;
    //             return (void *)(&temp[3]);
    //         }
    // #if MMAP_IN_SMALLSIZE
    //         else if (plen[1] >= MALLOC_HOTLEVEL && plen[2] >= MAPPING_POINT)
    //         {
    //             new_len = PAGE_ALIGN(new_len);
    //             int *temp;
    //             int fd = __create_fd(new_len);
    //             if (fd == -1)
    //             {
    //                 return NULL;
    //             }
    //             // this indicates its a result from malloc
    //             temp = (int *)mmap(NULL, new_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0); // MAP_PRIVATE | MAP_ANONYMOUS

    //             if (temp == MAP_FAILED)
    //             {
    //                 perror("Error mmapping the file");
    //                 return NULL;
    //             }
    //             memset(temp, 0, old_len);
    //             memcpy(temp, plen, old_len);
    //             temp[0] = fd;
    //             temp[1] = 0;
    //             temp[2] = size;
    //             free(plen);
    //             return (void *)(&temp[3]);
    //         }
    // #endif
    //         int *temp = (int *)realloc(plen, new_len + 1);
    //         temp[2] = size;
    // #if MMAP_IN_SMALLSIZE
    //         if (temp != plen)
    //         {
    //             temp[1] += SMALL_SIZE_COPY_INCREASE;
    //         }
    // #endif
    //         return (void *)(&temp[3]);
    //     }
    //     else
    //     {
    //         new_len = PAGE_ALIGN(new_len);
    //         int *temp;
    //         if (fd == -1) // current in first block
    //         {
    //             int fd = __create_fd(new_len);

    //             // this indicates its a result from malloc
    //             temp = (int *)mmap(NULL, new_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0); // MAP_PRIVATE | MAP_ANONYMOUS

    //             // fprintf(stderr, "Switching\n");

    //             if (temp == MAP_FAILED)
    //             {
    //                 perror("Error mmapping the file");
    //                 return NULL;
    //             }
    //             memset(temp, 0, old_len);
    //             memcpy(temp, plen, old_len);
    //             temp[0] = fd;
    //             temp[1] = 0;
    //             temp[2] = size;
    //             free(plen);
    //             return (void *)(&temp[3]);
    //         }
    //         else
    //         {
    // #if ENABLE_HUGLETLB
    //             if (plen[1] >= MMAP_HOTLEVEL && plen[2] >= HUGE_TLB_POINT)
    //             {
    //                 new_len = HUGE_PAGE_ALIGN(new_len);
    //             }
    // #endif

    //             if (ftruncate(fd, new_len) == -1)
    //             {
    //                 perror("Error in ftruncate");
    //                 return NULL;
    //             }

    //             plen = (int *)mremap(plen, old_len, new_len, MREMAP_MAYMOVE);

    //             // fprintf(stderr, "remap\n");

    //             // This was added in the 5.7 kernel as a new flag to mremap(2) called MREMAP_DONTUNMAP.
    //             //  This leaves the existing mapping in place after moving the page table entries.
    //             plen[0] = fd;
    //             plen[2] = size;
    //             return (void *)(&plen[3]);
    //         }
    //     }
}

void *_calloc(size_t nitems, size_t size) // Further look
{
    void *block = NULL;
    ;

    if (size <= 0)
    {
        perror("Wrong size");
    }
    else if (size <= HEADLESS)
    {
        block = calloc(nitems, size);
        return block;
    }
    else if (size <= MAPPING_POINT)
    {
        int *temp = (int *)block;
        size_t len = size + SMALL_OFFSET;
        temp = (int *)calloc(nitems, len);
        if (temp)
        {
            temp[0] = -size;
            return (void *)(&temp[1]);
        }
        else
        {
            perror("Error in calloc");
            return NULL;
        }
    }
    else
    {
        size_t len = size + OFFSET;
        int *temp = (int *)block;
        len = PAGE_ALIGN(len);
        int fd = -1;
        temp = (int *)calloc(nitems, len);

        if (temp)
        {
            temp[0] = fd;   // fd
            temp[1] = 0;    // counter
            temp[2] = size; // size
            return (void *)(&temp[3]);
        }
        else
        {
            perror("Error in malloc");
            return NULL;
        }
    }
    return NULL;
}

void _free(void *ptr)
{

    int size = check_size(ptr);
    // printf("size is %d \n",size);

    if (size <= HEADLESS && size >= 0)
    {
        free(ptr);
    }
    else if (size >= SWITCH_POINT)
    {
        int *plen = (int *)__go_to_head(ptr);
        munmap((void *)plen, plen[2]);
    }
    else // the free for the 16 KiB session seems problematic
    {
        int *temp;
        temp = (int *)ptr - 1;
        free(temp);
    }
}

int __create_fd_share(int size)
{
    pid_t tid = gettid();

    char root[] = "/";
    char tid_string[5];

    sprintf(tid_string, "%d", tid);

    int fd = shm_open(strcat(root, tid_string), O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR); // permision specified
    if (fd == -1)
    {
        perror("Error in shm_open");
        return -1;
    }

    if (ftruncate(fd, size) == -1)
    {
        perror("Error in ftruncate");
        return -1;
    }

    return fd;
}

// void *_malloc_share(size_t size)
// {

//     size_t len = size + sizeof(size) * 3;
//     int *temp;
//     len = PAGE_ALIGN(len);
//     int count = 0;
//     int fd = __create_fd_share(len);
//     if (fd == -1)
//     {
//         return NULL;
//     }

//     temp = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0); // a shared object version
//     if (temp == MAP_FAILED)
//     {
//         perror("Error in mmap\n");
//         return NULL;
//     }
//     else
//     {
//         temp = __assign_header(temp, count, size);
//         return (void *)(&temp[3]);
//     }
// }

// void *_realloc_share(void *ptr, size_t size)
// {

//     size_t old_len = grab_length(ptr) + sizeof(size) * 3;
//     size_t new_len = size + sizeof(size) * 3;

//     int *plen = (int *)__go_to_head(ptr);
//     int fd = plen[0];
//     int count = plen[1];

//     if (fd == -1)
//     {
//         return NULL;
//     }

//     if (ftruncate(fd, new_len) == -1)
//     {
//         perror("Error in ftruncate");
//         return NULL;
//     }

//     int *temp = (int *)mremap(plen, old_len, new_len, MREMAP_MAYMOVE);

//     temp = __assign_header(temp, count++, size);

//     free(plen);
//     return (void *)(&temp[3]);
// }
