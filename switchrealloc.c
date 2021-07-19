#include "switchrealloc.h"

void *switch_malloc(size_t size)
{ //In this malloc, switch between glibc malloc(<256Kb) and mmap(>256Kb)

    size_t len = size + sizeof(size) * 2; //two addition int block is needed

    int *temp;

    if (len <= KB * SWITCH_POINT)
    {
        temp = (int *)malloc(len + 1);
        //fprintf(stdout, "Malloc branch\n");
        if (temp)
        {
            temp[0] = -1;
            temp[1] = len;
            return (void *)(&temp[2]);
        }
        else
        {
            fprintf(stderr, "Error in malloc");
            return NULL;
        }
    }
    else
    {
        len = PAGE_ALIGN(len);
        int fd = create_fd(len);
        if (fd == -1)
        {
            return NULL;
        }

        temp = (int *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
        //MAP_PRIVATE | MAP_ANONYMOUS
        //fprintf(stdout, "Mmap branch\n");
        if (temp == MAP_FAILED)
        {
            fprintf(stderr, "Error in mmap\n");
            return NULL;
        }
        else
        {
            temp[0] = fd;
            temp[1] = len;
            return (void *)(&temp[2]);
        }
    }
}

void *switch_realloc(void *ptr, size_t size)
{ //once realloc need to allocate space larger than 256Kb it switch from realloc to mremap
    size_t old_len = grab_length(ptr);
    size_t new_len = size + sizeof(size) * 2;

    int *plen = (int *)ptr;
    plen--; //return to len block
    plen--; //return to fd block
    int fd = *plen;

    //printf("len is:%ld\n", len);

    if (plen == NULL)
    {
        return NULL;
    }

    if (new_len <= KB * SWITCH_POINT)
    {
        int *temp = (int *)realloc(plen, new_len + 1); //this seems better to use sbrk()
        temp[1] = new_len;                             //incrementing this part
        return (void *)(&temp[2]);
    }
    else
    {
        new_len = PAGE_ALIGN(new_len);
        int *temp;
        if (fd == -1) //current in first block
        {
            int fd = create_fd(new_len);
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
            memcpy(temp, plen, old_len);
            temp[0] = fd;
            temp[1] = new_len;
            free(plen);
            return (void *)(&temp[2]);
        }
        else
        {
            if (ftruncate(fd, new_len) == -1)
            {
                perror("Error in ftruncate");
                return NULL;
            }

            int *temp = (int *)mremap(plen, old_len, new_len, MREMAP_MAYMOVE);

            //This was added in the 5.7 kernel as a new flag to mremap(2) called MREMAP_DONTUNMAP.
            // This leaves the existing mapping in place after moving the page table entries.
            temp[0] = fd;
            temp[1] = new_len;
            return (void *)(&temp[2]);
        }
    }
}

void *switch_calloc(size_t nitems, size_t size) //Further look
{
    size_t len = nitems * size + sizeof(size) * 2;

    int *p = NULL;

    if (size <= KB * SWITCH_POINT)
    {
        p = (int *)calloc(nitems + 1, size);
        if (p)
        {
            p[0] = -1;
            p[1] = len;
            return (void *)(&p[2]);
        }
        else
        {
            fprintf(stderr, "Error in calloc");
            return NULL;
        }
    }
    else
    {
        return switch_malloc(nitems*size);
    }
}

void switch_free(void *ptr)
{
    size_t len = grab_length(ptr);

    int *plen = (int *)ptr;
    plen--;
    plen--;

    if (len <= KB * SWITCH_POINT + sizeof(len) * 2)
    {
        //fprintf(stdout, "Malloc branch\n");
        free((int *)plen);
    }
    else
    {
        //fprintf(stdout, "Mmap branch\n");
        munmap((void *)plen, len);
    }
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

int create_fd(int size)
{

    int fd = shm_open("/myregion", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR);
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