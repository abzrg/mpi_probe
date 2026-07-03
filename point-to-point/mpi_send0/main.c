#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define ARRAY_LEN(ARR) sizeof((ARR)) / sizeof((ARR)[0])

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank = {};
    int size = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    constexpr auto max_msg_size = 256;
    constexpr auto msg_tag = 0;
    constexpr auto sender = 0;
    constexpr auto recipient = 1;
    MPI_Status status = {0};

    if (rank == 0) {
        constexpr char msg[] = "I'm process 0 and I'm the king";
        MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, recipient, msg_tag,
                 MPI_COMM_WORLD);
        printf("Process %d sent the message: '%s'\n", rank, msg);
    } else {
        char msg[max_msg_size] = {0};
        // Note count is considered to be the maximum count
        MPI_Recv(&msg, max_msg_size, MPI_CHAR, sender, msg_tag, MPI_COMM_WORLD,
                 &status);
        printf("Process %d received the message: '%s'\n", rank, msg);
    }

    MPI_Finalize();

    return 0;
}
