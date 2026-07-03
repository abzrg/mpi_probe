#!/usr/bin/env bash

set -e

CC=mpicc
CFLAGS="-std=c23 -Wpedantic -Wall -Wextra -Wsign-conversion -Wshadow"

set -x

$CC $CFLAGS hello_world2_rank_1.c -o prog_rank_1
$CC $CFLAGS hello_world2_rank_4.c -o prog_rank_2
