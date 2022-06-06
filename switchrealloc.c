#include "switchrealloc.h"
// #include <linux/gfp.h>

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
        size_t now_size = malloc_usable_size(p); // big overhead by this operation
        return now_size - SMALL_OFFSET;
    }
    else
    {
        int *temp = (int *)p;
        temp = temp - 1;
        int size = *temp;
        return size;
    }
}

static size_t get_size(void *p)
{
    int *temp = (int *)p;
    temp = temp - 1;
    int size = *temp;
    if (size < 0)
    {
        return size * -1;
    }
    else
    {
        return size;
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

// static void _internal_free(void *ptr)
// {

//     int *plen = (int *)__go_to_head(ptr);
//     munmap((void *)plen, plen[2]);
// }

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
    else
    {
        size_t len = size + OFFSET;
        int *temp;
        len = PAGE_ALIGN(len);
        int fd = -1;
        fd = __create_fd(len);

        temp = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);

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

    if (size <= HEADLESS)
    {
        void *block = NULL;
        block = realloc(ptr, size);
        // memset(block, 0, size);           //maybe necessary
        return block;
    }

    size_t old_size = get_size(ptr);
    // printf("old_size is %ld \n", old_size);
    //   printf("new realloc size is %ld \n", size);

#if ENABLE_X2_ENHANCEMENT
    if (size - old_size * 2 == 0)
    {
        size = X2_ALIGN(size);
    }
#endif

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

        return block;
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
        else
        {
            temp = temp - 3; // move to head;

            int fd = temp[0];
            new_size = PAGE_ALIGN(new_size);

#if ENABLE_X2_ENHANCEMENT
            if (size == old_size * 2)
            {
                // plen[1] += MMAP_HOTLEVEL / 3;
                new_size = X2_ALIGN(new_size);
            }
#endif

#if ENABLE_HUGLETLB
            if (size >= HUGE_TLB_POINT) // && temp[1] >= MMAP_HOTLEVEL &&)
            {
                new_size = HUGE_PAGE_ALIGN(new_size + OFFSET);
            }
#endif

#if ENABLE_PREDICTION
            if (size >= old_size)
            {
                temp[1] += 1;
            }
            else
            {
                temp[1] -= 1;
                if (temp[1] < 1)
                    temp[1] = 0;
            }

#if ENABLE_UNSHRINK_NOW
            if (temp[1] >= UNSHRINK_THRESHOULD)
            {
                return (void *)(&temp[3]);
            }
#endif

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

    // alloc_pages(GFP_KERNEL,2);

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
    else
    {
        int *plen = (int *)__go_to_head(ptr);
        munmap((void *)plen, plen[2]);
    }
}

// int __create_fd_share(int size)
// {
//     pid_t tid = gettid();

//     char root[] = "/";
//     char tid_string[5];

//     sprintf(tid_string, "%d", tid);

//     int fd = shm_open(strcat(root, tid_string), O_CREAT | O_RDWR,
//                       S_IRWXO | S_IRUSR | S_IWUSR); // permision specified
//     if (fd == -1)
//     {
//         perror("Error in shm_open");
//         return -1;
//     }

//     if (ftruncate(fd, size) == -1)
//     {
//         perror("Error in ftruncate");
//         return -1;
//     }

//     return fd;
// }

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
