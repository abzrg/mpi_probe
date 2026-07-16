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
    constexpr int ndims = 2;
    int sizes[ndims] = {3, 3};
    int subsizes[ndims] = {2, 2};
    int starts[ndims] = {1, 1};
    int order = MPI_ORDER_C;
    MPI_Type_create_subarray(ndims, sizes, subsizes, starts, order, oldtype,
                             &newtype);
    MPI_Type_commit(&newtype);

    struct data buffer[3][3] = {};

    if (rank == SENDER) {
        // Initialize
        for (int i = 0; i < sizes[0]; i++) {
            for (int j = 0; j < sizes[1]; j++) {
                buffer[i][j].d = i*j;
                buffer[i][j].c = 'A' + i + j;
            }
        }

        printf("sizeof(buffer) = %zu\n", sizeof(buffer));
        printf("original array:\n");
        printf("[\n");
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                printf("  {%lf, %c}", buffer[i][j].d, buffer[i][j].c);
                if (j == 2) {
                    printf("\n");
                }
            }
        }
        printf("]\n");

        MPI_Send(buffer, 1, newtype, RECEIVER, 42, MPI_COMM_WORLD);
    }
    else if (rank == RECEIVER) {
        MPI_Recv(buffer, 1, newtype, SENDER, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        printf("subarray:\n");
        printf("[\n");
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                printf("  {%lf, %c}", buffer[i][j].d, buffer[i][j].c);
                if (j == 2) {
                    printf("\n");
                }
            }
        }
        printf("]\n");
    }

    MPI_Type_free(&newtype);
    MPI_Type_free(&oldtype);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
