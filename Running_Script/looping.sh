#!/bin/bash

for ((i=0;i<$1;i++))
do

echo "iteration $i"

#./../test $2 $3 $4 >> ../data/size/$2/$4.txt  #for data size

#./test $2 $3 $4 >> $3.txt #for parallel

#source test.sh $2 $3 $4 >> results.txt

#./test $2 $3 $4 >> $5.txt # for switch point testing

./../test $2 $3 $4 >> $2.txt  #for data size with same method

#./randtest >> $2.txt # for random test

sleep 10s

done


#$1: itreation counter
#$2: size  = 4Kb * (2^$2)
#$3: thread counter
#$4: mode selection, 0 for malloc, 1 for mmap , 2 for hybird version
