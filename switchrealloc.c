#include "switchrealloc.h"

void *_malloc(size_t size)
{ //In this malloc, switch between glibc malloc(Switch_point) and mmap(Switch_point) current switch point as 128KB

    size_t len = size + sizeof(size) * 3; //two addition int block is needed
    int count = 0;                        //for copy prediction
    int fd = -1;                          //for file_descriptor, which helps judge the mmap and malloc
    int *temp = NULL;

    if (len <= KB * SWITCH_POINT - AGGRESIVE * PAGE_SIZE)
    {
        temp = (int *)malloc(len + 1);
        //fprintf(stdout, "Malloc branch\n");
        if (temp)
        {
            temp = __assign_header(temp, fd, count, size);
            return (void *)(&temp[3]);
        }
        else
        {
            perror("Error in malloc");
            return NULL;
        }
    }
    else
    {
        len = PAGE_ALIGN(len);
        fd = __create_fd(len);
        if (fd == -1)
        {
            return NULL;
        }

        temp = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
        //fprintf(stdout, "Mmap branch\n");
        if (temp == MAP_FAILED)
        {
            perror("Error in mmap\n");
            return NULL;
        }
        else
        {
            temp = __assign_header(temp, fd, count, size);
            return (void *)(&temp[3]);
        }
    }
}

void *_realloc(void *ptr, size_t size)
{ //once realloc need to allocate space larger than 256Kb it switch from realloc to mremap
    size_t old_len = grab_length(ptr) + sizeof(size) * 3;
    size_t new_len = size + sizeof(size) * 3;

    int *plen = (int *)ptr;
    plen--; //return to len block
    plen--; //return to count block
    plen--; //return to fd block
    int fd = *plen;

    if (plen == NULL)
    {
        return NULL;
    }

    if ((new_len <= KB * SWITCH_POINT - AGGRESIVE * PAGE_SIZE) && (plen[1] <= MALLOC_HOTLEVEL || plen[2] <= MAPPING_POINT) && fd == -1)
    {
        int *temp = (int *)realloc(plen, new_len + 1); //this seems better to use sbrk()
        temp[1]++;
        temp[2] = size; //incrementing this part
        return (void *)(&temp[3]);
    }
    else
    {
        new_len = PAGE_ALIGN(new_len);
        int *temp = NULL;
        if (fd == -1) //current in first block
        {
            int fd = __create_fd(new_len);
            if (fd == -1)
            {
                return NULL;
            }
            //this indicates its a result from malloc
            temp = (int *)mmap(NULL, new_len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0); //MAP_PRIVATE | MAP_ANONYMOUS

            if (temp == MAP_FAILED)
            {
                perror("Error mmapping the file");
                return NULL;
            }
            memset(temp, 0, old_len);
            memcpy(temp, plen, old_len);
            temp[0] = fd;
            temp[1] = 0;
            temp[2] = size;
            free(plen);
            return (void *)(&temp[3]);
        }
        else
        {
            if (plen[1] >= MMAP_HOTLEVEL && ENABLE_HUGLETLB == 1 && plen[2] >= HUGE_TLB_POINT)
            {
                new_len = HUGE_PAGE_ALIGN(new_len);
            }

            if (ftruncate(fd, new_len) == -1)
            {
                perror("Error in ftruncate");
                return NULL;
            }

            int *temp = (int *)mremap(plen, old_len, new_len, MREMAP_MAYMOVE);

            //This was added in the 5.7 kernel as a new flag to mremap(2) called MREMAP_DONTUNMAP.
            // This leaves the existing mapping in place after moving the page table entries.
            temp[0] = fd;
            temp[1]++;
            temp[2] = size;
            return (void *)(&temp[3]);
        }
    }
}

void *_calloc(size_t nitems, size_t size) //Further look
{
    size_t len = nitems * size + sizeof(size) * 3;
    int count = 0;

    int *p = NULL;
    int fd = -1;

    if (size <= KB * SWITCH_POINT - AGGRESIVE * PAGE_SIZE)
    {
        p = (int *)calloc(len + 1, size);
        if (p)
        {
            p = __assign_header(p, fd, count, nitems * size);
            return (void *)(&p[3]);
        }
        else
        {
            perror("Error in calloc");
            return NULL;
        }
    }
    else
    {
        return _malloc(nitems * size);
    }
}

void _free(void *ptr)
{
    size_t len = grab_length(ptr) + sizeof(size_t) * 3;

    int *plen = (int *)__go_to_head(ptr);

    if (len <= KB * SWITCH_POINT + sizeof(len) * 3 - AGGRESIVE * PAGE_SIZE)
    {
        //fprintf(stdout, "Malloc branch\n");
        free(plen);
    }
    else
    {
        //fprintf(stdout, "Mmap branch\n");
        munmap((void *)plen, len);
    }
}

int *__assign_header(int *ptr, int fd, int count, int size)
{
    ptr[0] = -1;
    ptr[1] = count;
    ptr[2] = size;
    return ptr;
}

size_t grab_length(void *ptr)
{
    int *plen = (int *)ptr;
    size_t len;

    plen--;
    len = *plen;
    //printf("len in function is:%ld\n", len);
    return len;
}

void *__go_to_head(void *ptr)
{
    int *plen = (int *)ptr;
    plen--;
    plen--;
    plen--;
    return (void *)plen;
}

void *__go_to_size(void *ptr)
{
    int *plen = (int *)ptr;
    plen--;
    return (void *)plen;
}

int __create_fd(int size)
{

    int fd = shm_open("/myregion", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR); //permision specified
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

int __create_fd_share(int size)
{
    pid_t tid = gettid();

    char root[] = "/";
    char tid_string[5];

    sprintf(tid_string, "%d", tid);

    int fd = shm_open(strcat(root, tid_string), O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR); //permision specified
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

void *_malloc_share(size_t size)
{

    size_t len = size + sizeof(size) * 3;
    int *temp;
    len = PAGE_ALIGN(len);
    int count = 0;
    int fd = __create_fd_share(len);
    if (fd == -1)
    {
        return NULL;
    }

    temp = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0); //a shared object version
    if (temp == MAP_FAILED)
    {
        perror("Error in mmap\n");
        return NULL;
    }
    else
    {
        temp = __assign_header(temp, fd, count, size);
        return (void *)(&temp[3]);
    }
}

void *_realloc_share(void *ptr, size_t size)
{

    size_t old_len = grab_length(ptr) + sizeof(size) * 3;
    size_t new_len = size + sizeof(size) * 3;

    int *plen = (int *)__go_to_head(ptr);
    int fd = plen[0];
    int count = plen[1];

    if (fd == -1)
    {
        return NULL;
    }

    if (ftruncate(fd, new_len) == -1)
    {
        perror("Error in ftruncate");
        return NULL;
    }

    int *temp = (int *)mremap(plen, old_len, new_len, MREMAP_MAYMOVE);

    temp = __assign_header(temp, fd, count++, size);

    free(plen);
    return (void *)(&temp[3]);
}
