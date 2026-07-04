// source: https://hpc-tutorials.llnl.gov/mpi/examples/mpi_ringtopo.c
// author: Blaise Barney (04/02/05)

/*
    ...
    SENDER: Number of MPI tasks is: 4.
    Task 0 is partner with 2.
    Task 3 is partner with 1.
    Task 2 is partner with 0.
    Task 1 is partner with 3.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    enum role_ranks : uint32_t { SENDER };
    int taskid = {};
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    int numtasks = {};
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks % 2 != 0 && taskid == SENDER) {
        printf("Quitting. Need an even number of tasks: numtasks=%d\n", numtasks);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    char hostname[256] = {};
    int len = {};
    MPI_Get_processor_name(hostname, &len);
    printf("Hello from task %d on %s!\n", taskid, hostname);

    if (taskid == SENDER) {
        printf("SENDER: Number of MPI tasks is: %d.\n", numtasks);
    }

    // Determine partner and then send/receive with partner
    int partner = {};
    if (taskid < numtasks / 2) {
        partner = numtasks/2 + taskid;
    } else if (taskid >= (numtasks / 2)) {
        partner = taskid - numtasks / 2;
    }

    // 0: send, 1: receive
    MPI_Status stats[2] = {};
    MPI_Request reqs[2] = {};

    auto message = 42;
    auto msgcount = 1;
    auto msgtag = 1;
    MPI_Irecv(&message, msgcount, MPI_INT, partner, 1, MPI_COMM_WORLD, &reqs[0]);
    MPI_Isend(&taskid, 1, MPI_INT, partner, msgtag, MPI_COMM_WORLD, &reqs[1]);

    // Now block until requests are complete.
    MPI_Waitall(2, reqs, stats);

    // Print partner info and exit
    printf("Task %d is partner with %d.\n", taskid, message);

    MPI_Finalize();

    return 0;
}
