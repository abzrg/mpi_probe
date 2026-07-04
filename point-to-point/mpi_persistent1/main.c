// source: https://rookiehpc.org/mpi/docs/mpi_probe/index.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    enum role_ranks : uint32_t { SENDER, RECEIVER };
    int rank = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size = {};
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size % 2 != 0 && rank == SENDER) {
        printf("Quitting. Need at least two tasks: numtasks=%d\n", size);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    constexpr int count = 1;
    constexpr int tag = 42;
    MPI_Request request = {};

    if (rank == SENDER) {
        int buffer_sent = {};
        MPI_Send_init(&buffer_sent, count, MPI_INT, RECEIVER, tag,
                      MPI_COMM_WORLD, &request);

        for (int i = 0; i < 3; i++) {
            // Changing the buffer dynamically
            buffer_sent = 12345 + i;

            // Launch the send
            MPI_Start(&request);

            // Wait for the send to complete before printing
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            printf("MPI process %d sends value %d for message %d.\n", rank,
                   buffer_sent, i);
        }

        MPI_Request_free(&request);
    } else if (rank == RECEIVER) {
        int buffer_recvd;
        MPI_Recv_init(&buffer_recvd, count, MPI_INT, SENDER, tag,
                      MPI_COMM_WORLD, &request);

        for (int i = 0; i < 3; i++) {
            MPI_Start(&request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            printf("MPI process %d received value %d for message %d.\n", rank,
                   buffer_recvd, i);
        }

        MPI_Request_free(&request);
    }

    MPI_Finalize();

    return 0;
}
