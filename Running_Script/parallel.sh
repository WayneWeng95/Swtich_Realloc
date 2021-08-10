#!/bin/bash

for ((i=1;i<=$1;i++))
do

#./looping.sh 10 $2 $i 0     #for Malloc

./looping.sh 10 $2 $i 1    #for Mmap

sleep 10s

done


# $1 for threads
# $2 for size