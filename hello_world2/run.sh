#!/usr/bin/env bash

set -ex

# :OVERSUBSCRIBE enables us to spawn more processes than the number of
# processors available on the machine.
MPI_FLAGS="--map-by :OVERSUBSCRIBE"

# Meaning of colon (:)
#     Simultaneously run prog_rank_1 and prog_rank_2, with 3 and 5 processors,
#     respectively.
mpirun $MPI_FLAGS -np 3 ./prog_rank_1 : -np 5 prog_rank_2
