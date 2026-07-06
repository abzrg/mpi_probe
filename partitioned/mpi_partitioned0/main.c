#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    enum rank_roles : unsigned int { SENDER, RECEIVER };
    int rank = {};
    int size = {};

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr, "This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    constexpr size_t partition_count = 4;
    constexpr size_t partition_size = 10;
    constexpr size_t buffer_size = partition_count * partition_size;
    int buffer[buffer_size] = {};
    MPI_Request request = {};
    int flag = {};

    if (rank == SENDER) {
        MPI_Psend_init(&buffer, partition_count, partition_size, MPI_INT,
                       RECEIVER, 42, MPI_COMM_WORLD, MPI_INFO_NULL, &request);
        MPI_Start(&request);

        for (size_t i = 0; i < partition_count; i++) {
            const auto partition_start_index = i * partition_size;
            // Unique data for each partition
            for (size_t j = 0; j < partition_size; j++) {
                buffer[partition_start_index + j] = j * i;
            }

            MPI_Pready(i, request);
        }

        // Check for completion

#if 0
        MPI_Wait(&request, MPI_STATUS_IGNORE);
#else // OR
        while (!flag) {
            // Computations
            MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
            // Computations
        }
#endif

        MPI_Request_free(&request);
    } else if (rank == RECEIVER) {
        MPI_Precv_init(&buffer, partition_count, partition_size, MPI_INT,
                       SENDER, 42, MPI_COMM_WORLD, MPI_INFO_NULL, &request);
        MPI_Start(&request);

        // Check for completion

#if 0
        MPI_Wait(&request, MPI_STATUS_IGNORE);
#else // OR
        while (!flag) {
            // Computations
            MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
            // Computations
        }
#endif

        for (size_t i = 0; i < partition_count; i++) {
            const auto partition_start_index = i * partition_size;

            for (size_t j = 0; j < partition_size; j++) {
                printf("partition[%zu]->buffer[%zu] = %d\n", i,
                       partition_start_index + j,
                       buffer[partition_start_index + j]);
            }
        }

        MPI_Request_free(&request);
    }

    // Clean up

    MPI_Finalize();

    return EXIT_SUCCESS;
}
