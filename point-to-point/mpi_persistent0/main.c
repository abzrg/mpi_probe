#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    enum role_ranks : uint32_t { SENDER, RECEIVER };
    int taskid = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    int numtasks = {};
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks % 2 != 0 && taskid == SENDER) {
        printf("Quitting. Need at least two tasks: numtasks=%d\n", numtasks);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    constexpr int count = 1;
    int buffer = {};
    constexpr int tag = 42;
    MPI_Request request = {};
    MPI_Status status = {};

    if (taskid == SENDER) {
        buffer = 42;
        MPI_Send_init(&buffer, count, MPI_INT, RECEIVER, tag,
                      MPI_COMM_WORLD, &request);

        MPI_Start(&request);
        MPI_Wait(&request, &status);

        MPI_Request_free(&request);
    } else if (taskid == RECEIVER) {
        MPI_Recv_init(&buffer, count, MPI_INT, SENDER, tag,
                      MPI_COMM_WORLD, &request);

        MPI_Start(&request);
        MPI_Wait(&request, &status);
        printf("Process 1 received: %d.\n", buffer);

        MPI_Request_free(&request);
    }

    MPI_Finalize();

    return 0;
}
