#include "switchrealloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <threads.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#define INIT_SIZE 4096
#define SWITCH_OFFSET 3*sizeof(int)

thread_local clock_t t1, t2;
thread_local double total_time;
thread_local int *n;
int looping;

struct timeval start;
struct timeval end;
unsigned long timer;

void *mmap_thread();

void *mmaprelease_thread();

void *malloc_thread();

void *switch_thread();
