#include <stdio.h>
#include <mpi.h>
#include <string.h>


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank = {};
    int size = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    constexpr char msg[] = "I'm process 0 and I'm the king";
    constexpr auto recv_buf_max_size = 256;
    char recv_buf[recv_buf_max_size] = {0};

    constexpr auto sender = 0;
    constexpr auto recipient = 1;
    constexpr auto msg_tag = 42;
    MPI_Status status = {0};

    if (rank == sender) {
        // MPI_PROC_NULL: This rank may be used to send or receive from no-one.
        // Only send and don't care about recv?
        MPI_Sendrecv(&msg, strlen(msg) + 1, MPI_CHAR, recipient, msg_tag,
                     &recv_buf, recv_buf_max_size, MPI_CHAR, MPI_PROC_NULL,
                     msg_tag, MPI_COMM_WORLD, &status);
    } else if (rank == recipient) {
        MPI_Recv(&recv_buf, recv_buf_max_size, MPI_CHAR, sender, msg_tag,
                 MPI_COMM_WORLD, &status);
        printf("The message received was: %s.\n", recv_buf);
    }

    MPI_Finalize();

    return 0;
}
