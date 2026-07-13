program rowtype_vector

    use mpi

    implicit none

    integer, dimension(8, 8) :: array
    integer :: rank, ierr
    integer :: i, j
    integer :: rowtype

    call mpi_init(ierr)
    call mpi_comm_rank(MPI_COMM_WORLD, rank, ierr)

    ! initialize arrays
    if (rank == 0) then
        do i=1,8
            do j=1,8
                array(i, j) = i*10 + j;
            end do
        end do
    else
        array(:,:) = 0
    end if

    ! create datatype
    ! count:       8 -> each row has 8 elements that are separated from each other in physical memory
    ! blocklength: 1 -> there's only one item of the row in one column
    ! stride:      8 -> items in one column must be skipped to reach to the next item in the row
    call mpi_type_vector(8, 1, 8, MPI_INTEGER, rowtype, ierr)
    call mpi_type_commit(rowtype, ierr)

    ! send third row of matrix
    if (rank == 0) then
        call mpi_send(array(3,1), 1, rowtype, 1, 11, MPI_COMM_WORLD, ierr)
    ! recv at fifth row of matrix
    else if (rank == 1) then
        call mpi_recv(array(5,1), 1, rowtype, 0, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    end if

    ! print out results
    if (rank == 1) then
        do i=1,8
            write(*,'(8I3)') array(i, :)
        end do
    end if


    ! clean up

    call mpi_type_free(rowtype, ierr)

    call mpi_finalize(ierr)

end program rowtype_vector
