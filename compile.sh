#!/bin/bash

gcc main.c -g -o test -lrt -pthread -Wall

gcc randmain.c -g -o randtest -lrt -pthread -Wall

gcc tempmain.c -g -o temptest -lrt -pthread -Wall
