#include "switchrealloc.h"

thread_local clock_t t1, t2;
thread_local double total_time;
int looping;

struct  timeval  start;
struct  timeval  end;
unsigned long timer;

void *mmap_thread();

void *malloc_thread();

void *switch_thread();
