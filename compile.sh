#!/bin/zsh

gcc main.c -g -o test1 -lrt -pthread -Wall

gcc fileload.c -g -o test2 -lrt -pthread -Wall