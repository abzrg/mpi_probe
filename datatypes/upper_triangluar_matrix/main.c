#include <stdio.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    int rank = {};

    constexpr int count = 10;
    int a[count][count] = {};
    int disp[count] = {};
    int blocklen[count] = {};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype upper_type;

    for (size_t i = 0; i < count; i++) {
        disp[i] = count*i + i;
        blocklen[i] = count - i;
    }

    MPI_Type_indexed(count, blocklen, disp, MPI_INT, &upper_type);
    MPI_Type_commit(&upper_type);

    // Initialize data on first rank
    if (rank == 0) {
        for (size_t i = 0; i < count; i++) {
            for (size_t j = 0; j < count; j++) {
                a[i][j] = i*count + j;
            }
        }
    }

    if (rank == 0) {
        MPI_Send(&a[0], 1, upper_type, 1, 42, MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Recv(&a[0], 1, upper_type, 0, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    // Print out
    if (rank == 1) {
        for (size_t i = 0; i < count; i++) {
            for (size_t j = 0; j < count; j++) {
                printf("%8d", a[i][j]);
            }
            printf("\n");
        }
    }


    // Clean up

    MPI_Type_free(&upper_type);

    MPI_Finalize();

    return 0;
}
