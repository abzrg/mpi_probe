#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    constexpr size_t buffer_size = 5U;
    int buffer[buffer_size] = {};

    enum rank_roles : unsigned int { SENDER, RECEIVER };
    int rank = {};
    int size = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == SENDER) {
        for (size_t i = 0; i < buffer_size; i++) {
            buffer[i] = i;
        }

        MPI_Send(buffer, buffer_size, MPI_INT, RECEIVER, 42, MPI_COMM_WORLD);
    } else if (rank == RECEIVER) {
        MPI_Recv(buffer, buffer_size, MPI_INT, SENDER, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        for (size_t i = 0; i < buffer_size; i++) {
            printf("buffer[%zu] = %d\n", i, buffer[i]);
        }
    }


    MPI_Finalize();

    return 0;
}
