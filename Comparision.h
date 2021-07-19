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

#define PAGE_SIZE 4096
#define LOOPING 1024*1024

thread_local clock_t t1, t2;
thread_local double total_time;
int looping;

struct  timeval  start;
struct  timeval  end;
unsigned long timer;

void *mmap_thread();

void *malloc_thread();
