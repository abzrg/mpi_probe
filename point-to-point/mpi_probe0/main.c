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

    if (size < 2) {
        printf("Quitting. Need at least two tasks: numtasks=%d\n", size);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    constexpr int tag = 42;

    if (rank == SENDER) {
        int buffer[3] = {123, 456, 789};
        printf("Process %d: sending a message containing 3 ints (%d, %d, %d), "
               "but the receiver is not aware of the length.\n", rank,
               buffer[0], buffer[1], buffer[2]);
        MPI_Send(buffer, 3, MPI_INT, RECEIVER, tag, MPI_COMM_WORLD);
    } else if (rank == RECEIVER) {
        // Retrieve information about the incoming message
        MPI_Status status = {};
        MPI_Probe(SENDER, tag, MPI_COMM_WORLD, &status);
        printf("Process %d: obtained message status by probing it.\n", rank);

        // Get the number of integers in the message probed.
        int count = {};
        MPI_Get_count(&status, MPI_INT, &count); // status is needed here
        printf("\tcount = %d\n", count);

        // Allocate the buffer now that we know how many elements there are
        int* buffer = calloc(count, sizeof(*buffer));

        // Finally, receive the message
        MPI_Recv(buffer, count, MPI_INT, SENDER, tag, MPI_COMM_WORLD,
                 &status);

        printf("Process %d: received message with all %d ints:", rank, count);
        for(int i = 0; i < count; i++)
        {
            printf(" %d", buffer[i]);
        }
        printf(".\n");

        free(buffer);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
