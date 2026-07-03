#include <stdio.h>

#include "mpi.h"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Each processor has its own copy of the following code.
    // -> SMPD: single program multiple data

    int rank = {}; // Process ID
    int size = {}; // Number of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("proc %d/%d: Hello, World\n", rank+1, size);

    MPI_Finalize();

    return 0;
}
