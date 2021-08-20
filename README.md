# Swtich_Realloc
A memory allocator with the combination of malloc and mmap to reduce copy during memory reallocation

Current Work in Progress.

## Finished:
structure switching

mremap for larger space

aggressive switch

config.h

counter for prediction

## Todo:
Multithread improvements
Performance Testing


## Running CMD
for size growing  ./test1 (Increase)(Thread)(Mode) 0:malloc 1:mmap 2:_malloc

for random allocations ./test2 (Mode) 0:malloc 1:mmap 2:_malloc
