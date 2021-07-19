#!/bin/zsh

./test $1 $2 0 &
P1=$!
./test $1 $2 1 &
P2=$!
wait $P1 $P2
