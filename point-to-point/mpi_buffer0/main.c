#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdint.h>


int main(void)
{
    MPI_Init(NULL, NULL);

    int rank = {};
    int size = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    constexpr size_t data_count = 10;
    int* data = calloc(data_count, sizeof(*data));

    // Allocate send buffer beforehand
    int bufsize = data_count * sizeof(*data) + MPI_BSEND_OVERHEAD;
    int8_t* buf;
    // A buffer of bufsize bytes can now be used by MPI_Bsend on all
    // communicators, assuming only one message at a time is sent
    MPI_Buffer_attach(malloc(bufsize), bufsize);
    // bufsize is reduced to zero at this point.

    constexpr auto sender = 0;
    constexpr auto recipient = 1;
    constexpr auto msg_tag = 42;

    if (rank == 0) {
        for (size_t i = 0; i < 10; i++) {
            data[i] = (int)i;
        }

        MPI_Bsend(data, data_count, MPI_INT, recipient, msg_tag,
                  MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Recv(data, data_count, MPI_INT, sender, msg_tag,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (size_t i = 0; i < data_count; ++i) {
            printf("%d\n", data[i]);
        }
    }

    // Clean up

    MPI_Buffer_detach(&buf, &bufsize);

    free(data);
    free(buf);

    MPI_Finalize();

    return 0;
}
