#!/bin/bash

echo "This is the script for continous testing on the cloud"
echo "input <repeation> <thread> <mode>"

iteration = $1
thread = $2
mode = $3

for ((i=0;i<18;i++))
do


./looping.sh $iteration $i $2 $3


done