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
#define SWITCH_OFFSET 3 * sizeof(int)
#define SLEEP_TIME 0

#define RANDNO 500      //500 for nonstep test

thread_local clock_t t1, t2;
thread_local double total_time;
thread_local char *n;
int looping;
char filePath[] = "file.txt";
char sizePath[] = "smalldata.txt";
FILE *pFile;
int fSize, testBuff;
char *buffer;
char *test;

struct timeval start = (struct timeval){0};
struct timeval end = (struct timeval){0};
unsigned long timer;

void *mmap_thread();

void *mmaprelease_thread();

void *malloc_thread();

void *_malloc_thread();
