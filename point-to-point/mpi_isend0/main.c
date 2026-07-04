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

    // TODO: check size

    constexpr char message[] = "I'm process 0 and I'm the king.";

    MPI_Request request = {};

    if (rank == SENDER) {
        MPI_Isend(&message, strlen(message) + 1, MPI_CHAR, RECEIVER, 0, MPI_COMM_WORLD, &request);

        int flag = {};
        MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
        if (flag) {
            printf("Send operation completed.\n");
        } else {
            printf("Send operation not completed yet.\n");
        }

        // Make sure send is complete before proceeding.
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    } else if (rank == RECEIVER) {
        constexpr auto maxbufsize = 256;
        char buf[maxbufsize] = {};
        MPI_Irecv(&buf, maxbufsize, MPI_CHAR, SENDER, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &request);

        // Make sure the buffer is safe to read
        MPI_Wait(&request, MPI_STATUS_IGNORE);

        printf("The message received was: '%s'\n", buf);
    }

    MPI_Finalize();

    return 0;
}
