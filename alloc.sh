#!/bin/zsh

time ./executable/alloc

./cleaning.sh

sleep 5s

time LD_PRELOAD=/home/weikangweng/Documents/Thesis_expandation/impls/gperftools-2.9.1/.libs/libtcmalloc_minimal.so.4 ./executable/alloc

./cleaning.sh

sleep 5s

time LD_PRELOAD=/home/weikangweng/Documents/Thesis_expandation/impls/jemalloc-5.2.1/lib/libjemalloc.so ./executable/alloc

./cleaning.sh

sleep 5s

time LD_PRELOAD=/home/weikangweng/Documents/Thesis_expandation/impls/mimalloc/our/release/libmimalloc.so ./executable/alloc