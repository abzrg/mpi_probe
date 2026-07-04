#!/usr/bin/env bash

set -e

: ${NPROC:=4}

if (( NPROC % 2 != 0 )); then
    >&2 echo "This program requires an even number of processes."
    exit 1
fi

mpirun -np ${NPROC} ./a.out
