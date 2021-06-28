/* Prototype and definitions related to partitioned communication APIs */
#ifndef __MPIPCL__
#define __MPIPCL__
#include <mpi.h>

#define SENDER 0
#define RECEIVER 1

#define ACTIVE 1
#define INACTIVE 0

typedef struct _mpix_request {
  int state;
  int size;
  int side;
  int sendparts;
  int recvparts;
  int readycount;
  MPI_Request *request;
} MPIX_Request;

int MPIX_Psend_init(void *buf, int partitions, MPI_Count count,
		   MPI_Datatype datatype, int dest, int tag, MPI_Comm comm,
		   MPIX_Request *request);

int MPIX_Precv_init(void *buf, int partitions, MPI_Count count,
		   MPI_Datatype datatype, int src, int tag, MPI_Comm comm,
		   MPIX_Request *request);

int MPIX_Pready(int partition, MPIX_Request *request);

int MPIX_Pready_range(int partition_low, int partition_high, MPIX_Request *request);

int MPIX_Pready_list(int length, int array_of_partitions[], MPIX_Request *request);

int MPIX_Parrived(MPIX_Request *request, int partition, int *flag);

/* Other MPI functions to be updated */ 

int MPIX_Start(MPIX_Request *request);
int MPIX_Startall(int count, MPIX_Request array_of_requests[]);

int MPIX_Wait(MPIX_Request *request, MPI_Status *status);
int MPIX_Waitall(int count, MPIX_Request array_of_requests[],
		MPI_Status array_of_statuses[]);
int MPIX_Waitany(int count, MPIX_Request array_of_requests[],
		int *index, MPI_Status *status);
int MPIX_Waitsome(int incount, MPIX_Request array_of_requests[],
		 int *outcount, int array_of_indices[],
		 MPI_Status array_of_statuses[]);

int MPIX_Test(MPIX_Request *request, int *flag, MPI_Status *status);
int MPIX_Testall(int count, MPIX_Request array_of_requests[],
		int *flag, MPI_Status array_of_statuses[]);
int MPIX_Testany(int count, MPIX_Request array_of_requests[],
		int *index, int *flag, MPI_Status *status);
int MPIX_Testsome(int incount, MPIX_Request array_of_requests[],
		 int *outcount, int array_of_indices[],
		 MPI_Status array_of_statuses[]);

int MPIX_Request_free(MPIX_Request *request);

/* partitioned request cannot be cancelled  */
/* int MPIX_Cancel(MPIX_Request *request);  */

#endif
