// source: https://www.youtube.com/watch?v=MiRoxeh0KtY

#include <stdio.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    int rank = {};
    int array[8][8] = {};
    MPI_Datatype columntype = {};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Type_vector(8, 1, 8, MPI_INT, &columntype);
    MPI_Type_commit(&columntype);

    // Initialize arrays
    if (rank == 0) {
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 8; j++) {
                array[i][j] = i*8 + j;
            }
        }
    }

    // Send the third column
    if (rank == 0) {
        MPI_Send(&array[0][2], 1, columntype, 1, 42, MPI_COMM_WORLD);
    // Receive into fifth column
    } else if (rank == 1) {
        MPI_Recv(&array[0][5], 1, columntype, 0, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    // Print out
    if (rank == 1) {
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 8; j++) {
                printf("%8d", array[i][j]);
            }
            printf("\n");
        }
    }

    // Clean up

    MPI_Type_free(&columntype);

    MPI_Finalize();

    return 0;
}
