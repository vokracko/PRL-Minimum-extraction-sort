#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAG 0
#define SEND(id) MPI_Send(&number, 1, MPI_INT, id, TAG, MPI_COMM_WORLD)
#define RECV(number, id) MPI_Recv(&number, 1, MPI_INT, id, TAG, MPI_COMM_WORLD, &stat)


bool leaf(int i, int p_count)
{
	return i > (p_count/2);
}

int main(int argc, char * argv[])
{
	MPI_Status stat;
	int number;
	int left;
	int right;
	int cpu_id;
	int p_count;

	MPI_Comm_size(MPI_COMM_WORLD, &p_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &cpu_id);

	if(cpu_id == 0)
	{
		int c;
		int recv_id = 0;
		FILE * handle = fopen("numbers", "r");

		while((c = fgetc(handle)) != EOF)
		{
			SEND(recv_id++);
			
			if(recv_id == 0)
				printf("%d", number);
			else
				printf(" %d", number);
		}

		printf("\n");

		c = 0;

		while(recv_id++ < p_count)
			SEND(recv_id);

		fclose(handle);
	}


	if(leaf(cpu_id, p_count))
	{
		RECV(number, 0);
		SEND(cpu_id / 2);
	}
	else
	{
		RECV(left, cpu_id *  2);
		RECV(right, cpu_id * 2 + 1);
		
		number = left < right ? left : right;

		if(cpu_id == 0)
			// mam vysledek
		else
			SEND(cpu_id / 2);
	}
	
	return 0;
}
