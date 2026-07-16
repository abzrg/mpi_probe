#!/usr/bin/env bash

set -e

: ${NPROC:=2}

mpirun -np ${NPROC} ./a.out
