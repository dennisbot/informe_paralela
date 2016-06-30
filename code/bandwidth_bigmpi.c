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
	int rank, numprocs, src, dst;
	char *buf = NULL;
	double t1, t2, bw, bw_mb, bw_sum, T, dt;
	int i, iter_max;
	size_t errors;
	MPI_Count numBytes;

	iter_max = 10;
	numBytes = (MPI_Aint)atof(argv[1]);
	
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (numprocs != 2) 
    {
        printf("Use only 2 processes. \n");
        MPI_Finalize();
        return -1;
    }

    errors = 0;
    dst = 1;
    src = 0;

	MPI_Alloc_mem((MPI_Aint)numBytes, MPI_INFO_NULL, &buf);
    memset(buf, dst, (size_t)numBytes);
    
	if (rank == dst) 
    {
		for (i = 0; i < iter_max; i++)
		{
			MPIX_Send_x(buf, numBytes, MPI_CHAR, src, dst, MPI_COMM_WORLD);
			MPIX_Recv_x(buf, numBytes, MPI_CHAR, src, dst, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
    }
    else if (rank == src) 
    {
		for (i = 0; i < iter_max; i++)
		{
			t1 = MPI_Wtime();
			MPIX_Recv_x(buf, numBytes, MPI_CHAR, dst, dst, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPIX_Send_x(buf, numBytes, MPI_CHAR, dst, dst, MPI_COMM_WORLD);
			t2 = MPI_Wtime();
			dt = (t2 - t1);
			bw = (double)numBytes / (double)dt;
			bw_mb = bw/1e6;
			bw_sum += bw_mb;
		}
			
		printf("Avg Bandwidth = %.3lfMb/seg\n", bw_sum / (double)iter_max);
		printf("Array size used = %.3fGb\n", (double)numBytes / 1e9);
	}
    
    MPI_Free_mem(buf);

    if (rank == 0 && errors == 0) 
        printf("SUCCESS\n");

    MPI_Finalize();

    return 0;
}
