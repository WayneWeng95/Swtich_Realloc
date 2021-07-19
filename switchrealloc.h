#include "Comparision.h"

#define KB 1024
#define SWITCH_POINT 64
#define MS 1000000

#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

int mmap_flag = 0;

typedef struct switch_alloc_header
{
    int size;
    int fd;
}switch_alloc_header;

void *switch_malloc(size_t size);

void *switch_realloc(void *ptr, size_t size);

void *switch_calloc(size_t nitems, size_t size);

void free(void *ptr);

size_t grab_length(void *ptr);

int create_fd(int size);