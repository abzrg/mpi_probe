#include <stdio.h>
#include <assert.h>

#include "mpi.h"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // MPMD:
    //      multiple program (if (rank == ...)),
    //      multiple data (rank, size, ...)

    int rank = {}; // Process ID
    int size = {}; // Number of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    assert(size >= 4);

    // Let only one processor to print the number of prcessors.
    // In this program: the rank-1 processor
    if (rank == 4) {
        printf("[prog %d] Number of procs: %d\n", rank, size);
    }

    // Block all processor to wait for the rank-0 processor to print number of
    // processors, and only when rank-0 is done doing with that, remove the
    // barrier.
    MPI_Barrier(MPI_COMM_WORLD);

    printf("[prog 4] [proc %d/%d] Hello, World\n", rank+1, size);

    MPI_Finalize();

    return 0;
}



