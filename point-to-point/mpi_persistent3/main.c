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

    constexpr size_t persistent_request_count = 5;
    constexpr size_t buffer_size = 10;
    int buffers[persistent_request_count][buffer_size] = {};
    MPI_Request requests[persistent_request_count] = {};

    if (rank == SENDER) {
        for (size_t i = 0; i < persistent_request_count; i++) {
            MPI_Send_init(&buffers[i], buffer_size, MPI_INT, RECEIVER, 42,
                          MPI_COMM_WORLD, &requests[i]);
        }

        for (size_t i = 0; i < persistent_request_count; i++) {
            // Unique buffers in each persistent send
            for (size_t j = 0; j < buffer_size; j++) {
                buffers[i][j] = i * j;
            }

            // Start communication
            MPI_Start(&requests[i]);

            // Computations

            // No waiting at this stage
        }

        MPI_Waitall(persistent_request_count, requests, MPI_STATUSES_IGNORE);
    } else if (rank == RECEIVER) {
        for (size_t i = 0; i < persistent_request_count; i++) {
            MPI_Recv_init(&buffers[i], 10, MPI_INT, SENDER, 42, MPI_COMM_WORLD,
                          &requests[i]);
        }

        for (size_t i = 0; i < persistent_request_count; i++) {
            MPI_Start(&requests[i]);

            // Computations

            // No waiting at this stage
        }

        MPI_Waitall(persistent_request_count, requests, MPI_STATUSES_IGNORE);

        for (size_t i = 0; i < persistent_request_count; i++) {
            printf("---\n");
            for (size_t j = 0; j < buffer_size; j++) {
                printf("PC%zu : buffers[%zu] = %d\n", i, j, buffers[i][j]);
            }
        }
    }

    // Clean up

    for (size_t i = 0; i < persistent_request_count; i++) {
        MPI_Request_free(&requests[i]);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
