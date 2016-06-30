#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <mpi.h>

int main (int argc, char *argv[])
{
	double 
		buf[4];
	int 
		i,
		reps,
		tag,
		numprocs,
		rank,
		dest, 
		source,
		avgT,
		rc,
		n;
	double 
		local_sum,
		global_sum,
		t1,
		t2, 
		sumT;
	char 
		msg;
	MPI_Status 
		status;
	int
		*local_results,
		*global_v_a,
		*global_v_b,
		*local_v_a,
		*local_v_b;
	long
		global_numbytes,
		local_numbytes,
		global_size,
		local_size;
		
	MPI_Init(&argc,&argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (rank == 0)
	{
		buf[0] = global_size = (long)1e8;//atof(argv[1]);
		buf[1] = global_numbytes = global_size * sizeof(int);
			
		local_size = global_size / numprocs;
		buf[2] = local_size;
		buf[3] = local_numbytes = local_size * sizeof(int);
		
		MPI_Alloc_mem((MPI_Aint)global_numbytes, MPI_INFO_NULL, &global_v_a);
		MPI_Alloc_mem((MPI_Aint)global_numbytes, MPI_INFO_NULL, &global_v_b);

		for (i = 0; i < global_size; i++)
		{
			global_v_a[i] = rand() % 2;
			global_v_b[i] = rand() % 2;
		}
		
		printf("Testing with %ld integers\n", (long)global_size);
	}
	
	t1 = MPI_Wtime();
	
	MPI_Bcast(&buf[0], 4, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	global_size = buf[0];
	global_numbytes = buf[1];
	local_size = buf[2];
	local_numbytes = buf[3];
	
	MPI_Alloc_mem((MPI_Aint)local_numbytes, MPI_INFO_NULL, &local_v_a);
	MPI_Alloc_mem((MPI_Aint)local_numbytes, MPI_INFO_NULL, &local_v_b);
	
	MPI_Scatter( &global_v_a[0], 
				 local_size, 
				 MPI_INT, 
				 &local_v_a[0], 
				 local_size, 
				 MPI_INT, 
				 0, 
				 MPI_COMM_WORLD);

	MPI_Scatter( &global_v_b[0], 
				 local_size, 
				 MPI_INT, 
				 &local_v_b[0], 
				 local_size, 
				 MPI_INT, 
				 0, 
				 MPI_COMM_WORLD);

	local_sum = 0;
	for (i = 0; i < local_size; i++)
	{
		local_v_a[i] = sin(local_v_a[i]*local_v_a[i] + local_v_b[i]);
		local_v_b[i] = cos(local_v_b[i]*3.1415f/180.f);
		local_sum += local_v_a[i] * local_v_b[i];
	}

	MPI_Reduce(	&local_sum, 
				&global_sum, 
				1, 
				MPI_DOUBLE, 
				MPI_SUM, 
				0,
				MPI_COMM_WORLD);	

	t2 = MPI_Wtime();
	
	if (rank == 0)
	{
		/*printf("A:\n");
		for (i = 0; i < global_size; i++)
			printf("%d ", global_v_a[i]);
		printf("\n------------\n");
		
		printf("B:\n");
		for (i = 0; i < global_size; i++)
			printf("%d ", global_v_b[i]);
		printf("\n------------\n");
		
		printf("\nxxxxxxxxxxxx\n");
		printf("Result = %d\n", global_sum);
		printf("\nxxxxxxxxxxxx\n");*/
		
		printf("Time = %.3eseg.\n", t2 - t1);
	}
	
	if (rank == 0)
	{
		MPI_Free_mem(global_v_a);
		MPI_Free_mem(global_v_b);
	}
	MPI_Free_mem(local_v_a);
	MPI_Free_mem(local_v_b);
	
	MPI_Finalize();
	return 0;
}
