#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    enum rank_roles : unsigned int { SENDER, RECEIVER };
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr,
                "Aborting. This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    constexpr size_t point_count = 3;

    MPI_Datatype triple_int; // newDatatype
    MPI_Type_contiguous(point_count, MPI_INT, &triple_int); // MPI_INT: oldDataType
    MPI_Type_commit(&triple_int);

    if (rank == SENDER) {
        int points[point_count] = {15, 23, 6};
        MPI_Send(points, 1, triple_int, 1, 0, MPI_COMM_WORLD);
    } else if (rank == RECEIVER) {
        int points[point_count] = {};
        MPI_Recv(points, 1, triple_int, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Received: %d %d %d\n", points[0], points[1], points[2]);
    }

    // Clean up

    MPI_Type_free(&triple_int);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
