#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAG 0
#define EMPTY 10000
#define SEND(number, id) MPI_Send(&number, 1, MPI_INT, id, TAG, MPI_COMM_WORLD)
#define RECV(number, id) MPI_Recv(&number, 1, MPI_INT, id, TAG, MPI_COMM_WORLD, &stat)


bool leaf(int i, int n)
{
	return i >= (n-1);
}


int main(int argc, char * argv[])
{
	MPI_Status stat;
	int number;
	int cpu_id;
	int p_count;
	int n;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &cpu_id);

	n = (p_count+1)/2;

	if(cpu_id == 0)
	{
		int recv_id = p_count-1;
		FILE * handle = fopen("numbers", "r");

		while((number = fgetc(handle)) != EOF)
		{
			SEND(number, recv_id);

			if(recv_id == p_count-1)
				printf("%u", number);
			else
				printf(" %u", number);

			recv_id--;
		}

		printf("\n");
		number = EMPTY;

		while(recv_id >= n)
		{
			SEND(number, recv_id);
			recv_id--;
		}

		fclose(handle);
	}


	if(leaf(cpu_id, n) || n == 1)
		RECV(number, 0);
	else
		number = EMPTY;

	if(n == 1)
		printf("%d\n", number);

	for(int i = 0; i < 2*n+log2(n)-1; ++i)
	{
		int left = EMPTY;
		int right = EMPTY;

		if(leaf(cpu_id, n))
		{
			SEND(number, (cpu_id - 1) / 2);
			RECV(number, (cpu_id - 1) / 2);
		}
		else
		{
			RECV(left, cpu_id * 2 + 1);
			RECV(right, cpu_id * 2 + 2);

			if(number == EMPTY)
			{
				if(left < right)
				{
					number = left;
					left = EMPTY;
				}
				else
				{
					number = right;
					right = EMPTY;
				}
			}

			SEND(left, cpu_id * 2 + 1);
			SEND(right, cpu_id * 2 + 2);

			if(cpu_id != 0)
			{
				SEND(number, (cpu_id - 1) / 2);
				RECV(number, (cpu_id - 1) / 2);
			}

			if(cpu_id == 0 && number != EMPTY)
			{
				printf("%d\n", number);
				number = EMPTY;
			}

		}
	}

	MPI_Finalize();

	return 0;
}
