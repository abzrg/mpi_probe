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

    if (size < 2) {
        fprintf(stderr, "At least two process is required for this program\n");
        return EXIT_FAILURE;
    }

    constexpr int msg_tag = 0;
    constexpr MPI_Count msg_count = 1;
    const MPI_Datatype msg_type = MPI_INT;
    constexpr int sender = 0;
    constexpr int recipient = 1;
    MPI_Status recv_status = {0};

    if (rank == 0) {
        constexpr int msg = 12345;

        printf("MPI process %d sends value %d.\n", rank, msg);

        MPI_Send(&msg, msg_count, msg_type, recipient, msg_tag, MPI_COMM_WORLD);
    } else if (rank == 1) {
        int msg = {};
        MPI_Recv(&msg, msg_count, msg_type, sender, msg_tag, MPI_COMM_WORLD,
                 &recv_status);
        printf("MPI process %d receives value %d.\n", rank, msg);
        printf("Status:\n\terror = %d\n\tsource = %d\n\ttag = %d\n",
               recv_status.MPI_ERROR, recv_status.MPI_SOURCE,
               recv_status.MPI_TAG);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
