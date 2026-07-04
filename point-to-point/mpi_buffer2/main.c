#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int size = {};
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0 && size < 2) {
        fprintf(stderr, "This program requires at least two processors.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (rank == 0) {
        int msg[4] = {1, 2, 3, 4};
        int pack_size;
        MPI_Pack_size(4, MPI_INT, MPI_COMM_WORLD, &pack_size);

        int buffer_size = pack_size + MPI_BSEND_OVERHEAD;
        void *buffer = malloc(buffer_size);

        MPI_Buffer_attach(buffer, buffer_size);

        MPI_Bsend(msg, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);

        MPI_Buffer_detach(&buffer, &buffer_size);
        free(buffer);
    } else if (rank == 1) {
        int recv[4];
        MPI_Recv(recv, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received: %d %d %d %d\n", recv[0], recv[1], recv[2], recv[3]);
    }

    MPI_Finalize();
    return 0;
}
