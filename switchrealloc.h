#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <threads.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "Config.h"

#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a)-1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)
#define HUGE_PAGE_ALIGN(addr) ALIGN(addr, HUGE_PAGE_SIZE)

// #define O_CREAT 0100
// #define O_RDWR 02
#define MS 1000000
#define KB 1024
#define PAGE_SIZE getpagesize()

void *_malloc(size_t);

void *_realloc(void *, size_t);

void *_calloc(size_t, size_t);

void _free(void *);

size_t grab_length(void *);

void *__go_to_size(void *);

void *__go_to_count(void *);

void *__go_to_head(void *);

int *__assign_header(int *, int, int, int);

int __create_fd(int);
