#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <mpi.h>

struct data {
    double d;
    char c;
};

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    enum rank_roles : unsigned int { SENDER, RECEIVER };
    int rank = {};
    int size = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr,
                "Aborting. This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Datatype oldtype = {};
    int bls[2] = {1, 1};
    MPI_Aint disps[2] = {0, 8};
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_CHAR};
    MPI_Type_create_struct(2, bls, disps, types, &oldtype);
    MPI_Type_commit(&oldtype);

    MPI_Datatype newtype = {};
    int count = 2; // no. of blocks
    int bl = 3; // no. of elements of oldtype in each block
    MPI_Aint disparray[2] = {0, 48}; // no. of bytes difference between
                                     // start of each block
    MPI_Type_create_hindexed_block(count, bl, disparray, oldtype, &newtype);
    MPI_Type_commit(&newtype);

    if (rank == SENDER) {
        struct data buffer[6] = {
            {3.14, 'A'}, {1.7,  'B'}, {2.1,  'C'},
            {2.14, 'D'}, {1.2,  'E'}, {4.5,  'F'}
        };

        printf("sizeof(buffer) = %zu\n", sizeof(buffer));
        MPI_Send(buffer, 1, newtype, 1, 42, MPI_COMM_WORLD);
    }
    else if (rank == 1) {
        struct data buffer[6];

        MPI_Recv(buffer, 1, newtype, 0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < 6; i++)
        {
            printf("buffer[%d].d = %lf\n", i, buffer[i].d);
            printf("buffer[%d].c = %c\n", i, buffer[i].c);
        }
    }

    MPI_Type_free(&newtype);
    MPI_Type_free(&oldtype);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
