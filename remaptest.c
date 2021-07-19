#include "Comparision.h"

int main()
{

    // void *p;

    // p = malloc(4096);
    // if (p == NULL)
    // {
    //     printf("Error in malloc!");
    //     return -1;
    // }

    size_t size_of_mem = 4096;
    int fd = shm_open("/myregion", O_CREAT | O_RDWR,
                      S_IRWXO | S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error in shm_open");
        return -1;
    }

    if (ftruncate(fd, size_of_mem) == -1)
    {
        perror("Error in ftruncate");
        return -1;
    }

    void *shm_address = mmap(0, size_of_mem,
                             PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_SHARED, fd, 0);
    if (shm_address == MAP_FAILED)
    {
        perror("Error mmapping the file");
        return -1;
    }

    void *temp = mremap(shm_address, 4096, 10240, MREMAP_MAYMOVE);

    if (temp == (void *)-1)
    {
        printf("This process failed.");
        perror("Error on mremap()");
        return -1;
    }

    free(temp);
}