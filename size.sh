#!/bin/zsh

it=30

./looping.sh $it $1 1 1

sleep 30s

./looping.sh $it $1 1 0

sleep 30s

./looping.sh $it $1 1 2


#30 for the iterations