#!/bin/bash

it=$2

./looping.sh $it $1 1 2

sleep 30s

./looping.sh $it $1 1 1

sleep 30s

./looping.sh $it $1 1 0

# sleep 30s

# ./looping.sh $it $1 1 3


#30 for the iterations