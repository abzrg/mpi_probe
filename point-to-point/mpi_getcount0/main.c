#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank = {};
    int size = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    constexpr MPI_Count count = 1'000'000LL;
    // All processes have one copy of the buffer
    int* buffer = calloc(count, sizeof(*buffer));
    if (buffer == nullptr) {
        fprintf(stderr, "Memory allocation.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    constexpr auto msg_tag = 42;
    constexpr auto sender = 0;
    constexpr auto recipient = 1;

    if (rank == sender) {
        for (MPI_Count i = 0LL; i < count; ++i) {
            buffer[i] = (int)i;
        }

        // Returns only when the buffer safely can be used!
        // Either the message is received by the other end or the message is
        // buffered with the help of the MPI library somewhere.
        MPI_Send(buffer, count, MPI_INT, recipient, msg_tag, MPI_COMM_WORLD);
        printf("rank 0 : sent %lld int elements\n", (long long)count);
    } else if (rank == recipient) {
        MPI_Status status = {0};
        MPI_Count receivedCount = {};

        // Returns only when the message is fully received into the receiver
        // buffer.
        MPI_Recv(buffer, count, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &status);
        printf("rank 1 : received %lld int elements\n", (long long)count);

        printf("\tstatus error:  %d\n", status.MPI_ERROR);
        printf("\tstatus source: %d\n", status.MPI_SOURCE);
        printf("\tstatus tag:    %d\n", status.MPI_TAG);

        MPI_Get_count(&status, MPI_INT, (int*)&receivedCount);
        MPI_Get_elements_x(&status, MPI_INT, &receivedCount);
        printf("\tstatus count: %lld\n", (long long)receivedCount);
    }

    MPI_Finalize();
    return 0;
}

