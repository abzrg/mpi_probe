#include <stdlib.h>
#include <stdio.h>

#include <mpi.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    enum rank_roles { SENDER, RECEIVER };
    int rank = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size = {};
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr, "This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    constexpr size_t buffer_size = 10;
    int buffer[buffer_size] = {};
    constexpr size_t persistent_count = 5;
    MPI_Request request = {};

    if (rank == SENDER) {
        MPI_Send_init(&buffer, buffer_size, MPI_INT, RECEIVER, 42,
                      MPI_COMM_WORLD, &request);

        for (size_t i = 0; i < persistent_count; i++) {
            // Unique buffers in each persistent send
            for (size_t j = 0; j < buffer_size; j++) {
                buffer[j] = i * j;
            }

            // Start communication
            MPI_Start(&request);

            // COMPUTATION

            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    } else if (rank == RECEIVER) {
        MPI_Recv_init(buffer, 10, MPI_INT, SENDER, 42, MPI_COMM_WORLD,
                      &request);

        for (size_t i = 0; i < persistent_count; i++) {
            MPI_Start(&request);

            // COMPUTATIONS

            MPI_Wait(&request, MPI_STATUS_IGNORE);

            printf("---\n");
            for (size_t j = 0; j < buffer_size; j++) {
                printf("PC%zu : buffer[%zu] = %d\n", i, j, buffer[j]);
            }
        }
    }

    // Clean up

    MPI_Request_free(&request);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
