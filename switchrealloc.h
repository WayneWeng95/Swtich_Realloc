#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <threads.h>
#include <sys/time.h>

#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a)-1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

#define KB 1024
#define SWITCH_POINT 64
#define MS 1000000

#define PAGE_SIZE 4096
#define LOOPING 1024 * 1024

void *switch_malloc(size_t size);

void *switch_realloc(void *ptr, size_t size);

void *switch_calloc(size_t nitems, size_t size);

void switch_free(void *ptr);

size_t grab_length(void *ptr);

int create_fd(int size);