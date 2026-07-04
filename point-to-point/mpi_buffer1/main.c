#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

/**
 * @brief Illustrate how to allocates the memory buffer in which store
 * MPI_Bsend message.
 * @details This application requires 3 processes: 1 sender and 2 receivers. It
 * shows how to allocate memory for multiple messages of different sizes:
 * - 1 MPI_INT to send to receiver 1
 * - 2 MPI_INT to send to receiver 2
 *
 * It can be visualised as follows:
 *
 * <---- Destined to receiver 1 ---><---- Destined to receiver 2 ---->
 * +--------------------+-----------+--------------------+-----------+
 * | MPI_Bsend overhead | 1 MPI_INT | MPI_Bsend overhead | 2 MPI_INT |
 * +--------------------+-----------+--------------------+-----------+
 *
 * source: https://rookiehpc.org/mpi/docs/mpi_bsend_overhead/index.html
 **/

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size = {};
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    enum role_ranks : uint32_t { SENDER, RECEIVER_1, RECEIVER_2 };
    int rank = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == SENDER && size < 3) {
        fprintf(stderr, "This program requires at least 3 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    switch (rank) {
        case SENDER:
        {
            int message_1 = 1234;
            int message_2[2] = {567, 890};

            // Allocate enough space to issue 2 buffered send and their messages
            int buffer_size = (MPI_BSEND_OVERHEAD + sizeof(int)) +
                              (MPI_BSEND_OVERHEAD + 2 * sizeof(int));
            char* buffer = malloc(buffer_size);
            printf("Size of an MPI_Bsend overhead: %d bytes.\n",
                   MPI_BSEND_OVERHEAD);

            // Pass the buffer allocated to MPI so it uses it when we issue
            // MPI_Bsend
            MPI_Buffer_attach(buffer, buffer_size);

            // Issue the buffered send
            printf("[Process %d] I send value %d to process %d.\n", rank,
                   message_1, RECEIVER_1);
            MPI_Bsend(&message_1, 1, MPI_INT, RECEIVER_1, 0, MPI_COMM_WORLD);
            printf("[Process %d] I send values %d and %d to process %d.\n",
                   rank, message_2[0], message_2[1], RECEIVER_2);
            MPI_Bsend(&message_2, 2, MPI_INT, RECEIVER_2, 0, MPI_COMM_WORLD);

            // Detach the buffer no longer used (it will wait for MPI_Bsend
            // messages to be sent first)
            MPI_Buffer_detach(&buffer, &buffer_size);
            free(buffer);

            break;
        }
        case RECEIVER_1:
        {
            int buffer = {};
            MPI_Recv(&buffer, 1, MPI_INT, SENDER, MPI_ANY_TAG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            printf("[Process %d] I received value %d.\n", rank, buffer);

            break;
        }
        case RECEIVER_2:
        {
            int buffer[2] = {};
            MPI_Recv(&buffer, 2, MPI_INT, SENDER, MPI_ANY_TAG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            printf("[Process %d] I received values %d and %d.\n", rank,
                   buffer[0], buffer[1]);
        }
    }

    MPI_Finalize();

    return 0;
}
