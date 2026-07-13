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

    // ---- Step 1: describe "struct data" to MPI as data_type ----
    // struct data is NOT contiguous (it's hetergeneous data) in the MPI sense
    // from MPI's point of view: it's a mix of a double and a char, and MPI has
    // no idea how your compiler laid it out. MPI_Type_create_struct tells MPI
    // the exact byte offset and MPI type of each field, so MPI can pack/
    // unpack the struct correctly regardless of compiler padding rules.
    constexpr size_t data_field_count = 2U;

    // one element per field (not "no gaps" - just "1 double" and "1 char")
    int data_blocklengths[data_field_count] = {1, 1};

    // byte offset of each field within struct data (accounts for compiler
    // padding, e.g. char 'c' may not start right after the 8-byte double)
    MPI_Aint data_displacements[data_field_count] = {
        offsetof(struct data, d), // 0
        offsetof(struct data, c)  // 8
    };
    MPI_Datatype data_blocktypes[data_field_count] = {MPI_DOUBLE, MPI_CHAR};

    // data_type = the "oldtype" building block used below by vector_type.
    // This is our custom struct type, analogous to a native type like
    // MPI_DOUBLE, but describing one { double; char; } pair.
    MPI_Datatype data_type = {};
    MPI_Type_create_struct(data_field_count, data_blocklengths,
                           data_displacements, data_blocktypes, &data_type);
    MPI_Type_commit(&data_type);

    // ---- Step 2: describe a strided PATTERN of data_type elements ----
    // MPI_Type_vector picks out repeating blocks of data_type, separated
    // by a fixed gap - unlike MPI_Type_contiguous, which assumes no gaps
    // at all (blocklength == stride, back-to-back).

    // No. of blocks (NOTE: different meaning than "count" in
    // MPI_Type_contiguous, where count is just "how many repeats")
    constexpr int vector_count = 2;

    // No. of data_type elements in each block (the "wanted" part)
    constexpr int vector_blocklength = 3;

    // No. of data_type elements between the START of one block and the
    // START of the next. Since stride (4) > blocklength (3), there is a
    // gap of (4 - 3 = 1) data_type element skipped between blocks.
    int vector_stride = 4;

    // vector_type = newtype, built out of data_type (our MPI struct type)
    MPI_Datatype vector_type = {};
    MPI_Type_vector(vector_count, vector_blocklength, vector_stride, data_type,
                    &vector_type);
    MPI_Type_commit(&vector_type);

    constexpr size_t buffer_size = 12;

    if (rank == SENDER) {
        // Indices:      0     1     2     3     4     5     6     7 ...
        // Block map:  [--- block 0 ---][gap][--- block 1 ---][ untouched ]
        // vector_type selects indices 0,1,2, (skips 3), 4,5,6
        // -> 6 structs total are actually sent, in that order.
        struct data buffer[buffer_size] = {
            {3.14, 'A'}, {1.7,  'B'}, {2.1,  'C'}, {2.14, 'D'},
            {1.2,  'E'}, {4.5,  'F'}, {3.14, 'G'}, {1.7,  'H'},
            {2.1,  'I'}, {4.5,  'J'}, {3.14, 'K'}, {1.7,  'L'},
        };

        printf("sizeof(buffer) = %zu\n", sizeof(buffer));

        // count=1 here means "1 instance of vector_type", NOT "1 struct".
        // vector_type itself already encodes "6 structs, with a gap".
        MPI_Send(buffer, 1, vector_type, RECEIVER, 42, MPI_COMM_WORLD);
    }
    else if (rank == RECEIVER) {
#if 0
        // Option A: receive back into the SAME strided layout as the
        // sender's array. Using vector_type on the receive side writes
        // into indices 0,1,2,4,5,6 of a 12-element buffer, mirroring the
        // sender's gaps. Indices 3, 7-11 are never written by MPI, so
        // they stay whatever they were initialized to (zeroed here by
        // `= {}`, but would be garbage without that initializer).
        struct data buffer[buffer_size] = {};
        MPI_Recv(buffer, 1, vector_type, SENDER, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        for (size_t i = 0; i < buffer_size; i++) {
            printf("buffer[%zu].d = %f\n", i, buffer[i].d);
            printf("buffer[%zu].c = %c\n", i, buffer[i].c);
        }
#else
        // Option B: receive into a TIGHTLY PACKED buffer of exactly the 6
        // structs that were actually sent. MPI sends the selected
        // elements as a contiguous stream "on the wire" regardless of the
        // gaps in the sender's memory, so the receiver doesn't need
        // vector_type at all here - a plain count of data_type is enough.
        constexpr size_t received_count = vector_count * vector_blocklength;
        struct data buffer[received_count] = {};
        MPI_Recv(buffer, received_count, data_type, SENDER, 42, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        for (size_t i = 0; i < received_count; i++) {
            printf("buffer[%zu].d = %f\n", i, buffer[i].d);
            printf("buffer[%zu].c = %c\n", i, buffer[i].c);
        }
#endif
    }

    // Clean up
    MPI_Type_free(&vector_type);
    MPI_Type_free(&data_type);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
