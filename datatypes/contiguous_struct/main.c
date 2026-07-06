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
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr,
                "Aborting. This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Create old type (the building block of new type (triple_data_type))
    constexpr size_t data_field_count = 2U;
    int data_blocklengths[data_field_count] = {1, 1};
    // Create a Typemap for data_type
    MPI_Aint data_displacements[data_field_count] = {
        offsetof(struct data, d), // 0
        offsetof(struct data, c)  // 8
    };
    MPI_Datatype data_blocktypes[data_field_count] = {MPI_DOUBLE, MPI_CHAR};

    MPI_Datatype data_type = {}; // oldtype (created out of a struct)
    MPI_Type_create_struct(data_field_count, data_blocklengths,
                           data_displacements, data_blocktypes, &data_type);
    MPI_Type_commit(&data_type);

    // Create new type
    MPI_Datatype triple_type = {}; // newtype (created out of a MPI type)
    MPI_Type_contiguous(3, data_type, &triple_type);
    MPI_Type_commit(&triple_type);

    constexpr size_t buffer_size = 3U;
    struct data buffer[buffer_size];

    if (rank == SENDER) {
        for (size_t i = 0; i < buffer_size; i++) {
             buffer[i].d = 3.1415 * i;
             buffer[i].c = 'A' + i;
        }

        constexpr size_t data_type_padded_size = sizeof(struct data);
        constexpr size_t data_type_packed_size
            = sizeof(buffer[0].d) + sizeof(buffer[0].c); // should be 9
        if (data_type_padded_size != (data_type_packed_size)) {
            printf("We have padding!\n");
            printf("sizeof(struct data) = %zu\n", data_type_padded_size);
            printf("which means: extent = %zu\n", data_type_padded_size);
        }

        int triple_type_size = {};
        MPI_Type_size(triple_type, &triple_type_size);

        // Does it include the padding?
        printf("MPI_Type_size(newtype) = %d\n", triple_type_size);

        // 1: buffer contains one item of type newtype
        MPI_Send(buffer, 1, triple_type, RECEIVER, 42, MPI_COMM_WORLD);
    }
    else if (rank == RECEIVER) {
        MPI_Recv(buffer, 1, triple_type, SENDER, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        for (size_t i = 0; i < buffer_size; i++) {
            printf("buffer[%zu].d = %lf\n", i, buffer[i].d);
            printf("buffer[%zu].c = %c\n", i, buffer[i].c);
        }
    }

    // Clean up

    MPI_Type_free(&triple_type);
    MPI_Type_free(&data_type);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
