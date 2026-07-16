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
    constexpr int count_old = 2;
    int bls_old[count_old] = {1, 1};
    MPI_Aint disps_old[count_old] = {0, 8};
    MPI_Datatype types_old[count_old] = {MPI_DOUBLE, MPI_CHAR};
    MPI_Type_create_struct(count_old, bls_old, disps_old, types_old, &oldtype);
    MPI_Type_commit(&oldtype); // probably no need to commit it we use it in a
                               // send/recv operation

    MPI_Datatype newtype = {};
    constexpr int count_new = 2; // no. of blocks
    int bls_new[count_new] = {3, 3}; // no. of elements of oldtype in each
                                     // block
    MPI_Aint disps_new[count_new] = {0, 48}; // no. of bytes difference between
                                             // start of each block
    MPI_Datatype types_new[count_old] = {oldtype, oldtype};
    MPI_Type_create_struct(count_new, bls_new, disps_new, types_new, &newtype);
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
