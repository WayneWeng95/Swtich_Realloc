#include "switchrealloc.h"

// static size_t get_size(void *ptr)
// {
//     int *temp = (int *)ptr;
//     temp = temp - 1;
//     size_t size = temp[0];
//     printf("%ld\n", size);
//     return size;
// }

// static void *__go_to_head(void *ptr)
// {
//     int *plen = (int *)ptr;
//     plen = plen - 3;
//     return (void *)plen;
// }

// static void *__go_to_head_realloc(void *ptr)
// {
//     int *plen = (int *)ptr;
//     plen = plen - 1;
//     return (void *)plen;
// }

static int __create_fd(size_t size)
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
    int *block;

    if (size <= SWITCH_POINT)
    {
        size_t len = size + SMALL_OFFSET;
        block = malloc(len + 1);
        block[0] = size;
        return (void *)(&block[1]);
    }
    else
    {
        size_t len = size + OFFSET;
        len = PAGE_ALIGN(len);
        int fd = -1;
        fd = __create_fd(len);

        block = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);

        if (block == MAP_FAILED)
        {
            perror("Error in mmap\n");
            return NULL;
        }
        else
        {
            block[0] = fd;   // fd
            block[1] = 0;    // counter
            block[2] = size; // size
            return (void *)(&block[3]);
        }
    }
    return NULL;
}

void *_realloc(void *ptr, size_t size)
{ // once realloc need to allocate space larger than 128Kb it switch from realloc to mremap

    if (size <= 0)
    {
        perror("Wrong size\n");
    }

    int *temp = (int *)ptr;
    temp = temp - 1;
    size_t old_size = temp[0];
    size_t old_len;
    if (old_size <= SWITCH_POINT)
    {
        old_len = old_size + SMALL_OFFSET;
    }
    else
    {
        old_len = old_size + OFFSET;
        temp = temp -2;
    }

#if ENABLE_X2_ENHANCEMENT
    if (size - old_size * 2 == 0)
    {
        size = X2_ALIGN(size);
    }
#endif
    void *block = NULL;

    // if (old_size <= SWITCH_POINT) // Got the correct memory pointer file.
    // {
    //     block = __go_to_head_realloc(ptr);
    // }
    // else
    // {
    //     block = __go_to_head(ptr);
    // }

    if (size <= SWITCH_POINT)
    {
        size_t new_len = size + SMALL_OFFSET;
        if (old_size <= SWITCH_POINT) // old size <= Switch && new size <= Switch, realloc
        {
            temp = realloc(temp, new_len);
            temp[0] = size;
            // memset(block, 0, size);           //maybe necessary
            return (void *)(&temp[1]);
        }
        else // old size > Switch && new size <= Switch, malloc-copy-free
        {
            block = _malloc(size);
            memcpy(block, ptr, size); // check copy length      check the ptr address
            temp = temp + 3;
            _free(temp);
            // memset(block, 0, size);           //maybe necessary
            return temp;
        }
    }
    else
    {
        if (old_size <= SWITCH_POINT) // old size <= Switch && new size > Switch, mmap-copy-free
        {
            block = _malloc(size);
            memcpy(block, ptr, old_size);
            temp = temp + 1;
            _free(temp);
            return block;
        }
        else // old size > Switch && new size > Switch, mremap
        {

            int fd = temp[0];
            size_t new_len = size + OFFSET;

#if ENABLE_X2_ENHANCEMENT
            if (size == old_size * 2)
            {
                // plen[1] += MMAP_HOTLEVEL / 3;
                new_len = X2_ALIGN(new_len);
            }
#endif

#if ENABLE_HUGLETLB                     // mmap a new area and move the content
            if (size >= HUGE_TLB_POINT) // && temp[1] >= MMAP_HOTLEVEL &&)
            {
                //mmap a new hugeTLB entry
                new_len = HUGE_PAGE_ALIGN(new_len);
            }
#endif

#if ENABLE_PREDICTION
            if (size >= old_size)
            {
                block[1] += 1;
            }
            else
            {
                block[1] -= 1;
                if (block[1] < 1)
                    block[1] = 0;
            }

#if ENABLE_UNSHRINK_NOW
            if (block[1] >= UNSHRINK_THRESHOULD)
            {
                return (void *)(&block[3]);
            }
#endif

#endif

            if (ftruncate(fd, new_len) == -1)
            {
                perror("Error in ftruncate");
                return NULL;
            }

            temp = (int *)mremap(temp, old_len, new_len, MREMAP_MAYMOVE);

            // fprintf(stderr, "remap\n");

            // This was added in the 5.7 kernel as a new flag to mremap(2) called MREMAP_DONTUNMAP.
            //  This leaves the existing mapping in place after moving the page table entries.
            temp[0] = fd;
            temp[2] = size;
            return (void *)(&temp[3]);
        }
    }

    return NULL;
}

void *_calloc(size_t nitems, size_t size) // Further look
{
    int *block;

    if (size <= 0)
    {
        perror("Wrong size");
    }
    else if (size <= HEADLESS)
    {
        block = (int *)calloc(nitems + 1, size);
        block[0] = size;
        return (void *)(&block[1]);
    }
    else
    {
        int fd = -1;
        block = (int *)calloc(nitems + 3, size);

        if (block)
        {
            block[0] = fd;   // fd refixing the fd
            block[1] = 0;    // counter
            block[2] = size; // size
            return (void *)(&block[3]);
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

    int *temp = (int *)ptr;
    temp = temp - 1;
    size_t size = temp[0];
    printf("size is %ld \n", size);

    if (size <= SWITCH_POINT)
    {
        free(temp);
    }
    else
    {
        temp = temp - 2;
        munmap((void *)temp, temp[2] + OFFSET);
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
