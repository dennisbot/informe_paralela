#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <strings.h>

#include <mpi.h>
#include "bigmpi.h"
#include "verify_buffer.h"

int main(int argc, char * argv[])
{
    int 
		rank, 
		nprocs;
	double 
		t1, 
		t2;
	char 
		*buf;
	MPI_Count 
		n;

	n = (MPI_Count)2e9;
		
	MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (nprocs < 2) 
    {
        printf("Use 2 or more processes. \n");
        MPI_Finalize();
        return -1;
    }

    buf = NULL;
    MPI_Alloc_mem((MPI_Aint)n, MPI_INFO_NULL, &buf);
    memset(buf, rank, (size_t)n);

	MPI_Barrier(MPI_COMM_WORLD);
	t1 = MPI_Wtime();
    for (int r = 1; r < nprocs; r++) 
    {
        if (rank == r)
            MPIX_Send_x(buf, n, MPI_CHAR, 0, r, MPI_COMM_WORLD);
        else if (rank == 0) 
            MPIX_Recv_x(buf, n, MPI_CHAR, r, r, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    t2 = MPI_Wtime();

	if (rank == 0)
	{
		printf("Bytes = %ld\n", (long)n);
		printf("Total Time = %.3e\n", t2 - t1);
	}

    MPI_Free_mem(buf);

    MPI_Finalize();

    return 0;
}
