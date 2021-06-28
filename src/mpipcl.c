/* Initial implementation of partitioned communication API */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mpipcl.h"


int MPIX_Psend_init(void *buf, int partitions, MPI_Count count,
		    MPI_Datatype datatype, int dest, int tag, MPI_Comm comm,
		    MPIX_Request *request){
  MPI_Aint lb, extent, offset;
  int i;
  
  /* update partitioned request object */
  request -> state = INACTIVE;
  request -> size = partitions;
  request -> side = SENDER;
  request -> sendparts = partitions;
  request -> recvparts = partitions; /* we are making this assumption */
  request -> request = malloc(partitions * sizeof (MPI_Request));
  assert(request -> request != NULL);
  
  /* create a persistent send request for each partition */
  /* NOTE: Assumes # of partitions on the sender and receiver are equal */
  assert(MPI_Type_get_extent(datatype, &lb, &extent) == MPI_SUCCESS);
  for (i = 0; i < partitions; i++) {
    offset = i * count * extent;
    assert(MPI_Send_init(buf + offset, count, datatype, dest, tag+i, comm, &request->request[i]) == MPI_SUCCESS);
  }
  
  return MPI_SUCCESS;
}

int MPIX_Precv_init(void *buf, int partitions, MPI_Count count,
		    MPI_Datatype datatype, int src, int tag, MPI_Comm comm,
		    MPIX_Request *request) {
  MPI_Aint lb, extent, offset;
  int i;
  
  /* update partitioned request object */
  request -> state = INACTIVE;
  request -> size = partitions;
  request -> side = RECEIVER;
  request -> recvparts = partitions; 
  request -> sendparts = partitions; /* we are making this assumption */
  request -> request = malloc(partitions * sizeof (MPI_Request));
  assert(request -> request != NULL);
  
  /* create a persistent receive request for each partition */
  /* NOTE: Assumes # of partitions on the sender and receiver are equal */
  assert(MPI_Type_get_extent(datatype, &lb, &extent) == MPI_SUCCESS);
  for (i = 0; i < partitions; i++) {
    offset = i * count * extent;
    assert(MPI_Recv_init(buf + offset, count, datatype, src, tag+i, comm, &request->request[i]) == MPI_SUCCESS);
  }
  
  return MPI_SUCCESS;
}

int MPIX_Pready(int partition, MPIX_Request *request) {
  /* TODO: check if this is invoked on the receiver,
           check if this is invoked on an active request, and
           throw appropriate error message. */

  return MPI_Start(&request->request[partition]);
  
}

int MPIX_Pready_range(int partition_low, int partition_high, MPIX_Request *request) {
  /* TODO: check if this is invoked on the receiver,
           check if this is invoked on an active request, and
           throw appropriate error message. */

  return MPI_Startall(partition_high-partition_low+1, &request->request[partition_low]);
  
}

int MPIX_Pready_list(int length, int array_of_partitions[], MPIX_Request *request) {
  /* TODO: check if this is invoked on the receiver,
           check if this is invoked on an active request, and
           throw appropriate error message. */

  int i;
  for (i = 0; i < length; i++)
    assert(MPI_Start(&request->request[array_of_partitions[i]]) == MPI_SUCCESS);
  
  return MPI_SUCCESS;
}

int MPIX_Parrived(MPIX_Request *request, int partition, int *flag) {
  /* TODO: check if this is invoked on the sender and
           throw appropriate error message. */
  
  return MPI_Test(&request->request[partition], flag, MPI_STATUS_IGNORE);
}


//example to fix above
int MPIX_Start(MPIX_Request *request){

  request -> state = ACTIVE;
  /* nothing to do on send side */
  if (request->side == RECEIVER)
    assert(MPI_Startall(request->size, request->request) == MPI_SUCCESS);

  return MPI_SUCCESS;
}

int MPIX_Startall(int count, MPIX_Request array_of_requests[]){

  int i;
  for (i = 0; i  < count; i++)
    assert(MPIX_Start(&array_of_requests[i]) == MPI_SUCCESS);

  return MPI_SUCCESS;
}

int MPIX_Wait(MPIX_Request *request, MPI_Status *status){

  assert(MPI_Waitall(request->size, request->request, MPI_STATUSES_IGNORE) == MPI_SUCCESS);
  request -> state = INACTIVE;
  /* NOTE: MPI_Status object is not updated */

  return MPI_SUCCESS;
}

int MPIX_Waitall(int count, MPIX_Request array_of_requests[],
                MPI_Status array_of_statuses[]){
  int i;
  for (i = 0; i < count; i++) {
    assert(MPIX_Wait(&array_of_requests[i], MPI_STATUS_IGNORE) == MPI_SUCCESS);
  }
  /* NOTE: array of MPI_Status objects is not updated */

  return MPI_SUCCESS; 
}

int MPIX_Waitany(int count, MPIX_Request array_of_requests[],
                int *index, MPI_Status *status){

  int i, flag=0;
  while (!flag) {
    for (i = 0; i < count; i++) {
      assert(MPIX_Test(&array_of_requests[i], &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS);
      if (flag == 1) {
        *index = i;
        break;
      }
    }
  }
  /* NOTE: MPI_Status object is not updated */

  return MPI_SUCCESS; 
}


//local attempt
int MPIX_Waitsome(int incount, MPIX_Request array_of_requests[],
                 int *outcount, int array_of_indices[],
                 MPI_Status array_of_statuses[]){
  int j=0, flag = 0;

  *outcount = 0;
  while(*outcount < 1){
  	for(int i = 0; i<incount; i++){
		flag == 0; //inforced reset should be reset by MPIX_Test call?? 
		assert(MPIX_Test(&array_of_requests[i], &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS);
	  	if(flag == 1){
	  	     *outcount = *outcount+1;
	  	     array_of_indices[j] = i;
	  	     j++;
	  	}
	}
  }
  return MPI_SUCCESS; 
}

int MPIX_Test(MPIX_Request *request, int *flag, MPI_Status *status){
  
  int i, myflag;
  *flag = 1;
  for (i = 0; i < request->size; i++) {
    assert(MPI_Test(&request->request[i], &myflag, MPI_STATUS_IGNORE) == MPI_SUCCESS);
    *flag = *flag & myflag;
  }

  if (*flag == 1) request -> state = INACTIVE;

  /* NOTE: MPI_Status object is not updated */
  return MPI_SUCCESS; 
}

int MPIX_Testall(int count, MPIX_Request array_of_requests[],
                int *flag, MPI_Status array_of_statuses[]){
  int i, myflag;
  *flag = 1;
  for (i = 0; i < count; i++) {
     assert(MPIX_Test(&array_of_requests[i], &myflag, MPI_STATUS_IGNORE) == MPI_SUCCESS);
     *flag = *flag & myflag;
  }

  /* NOTE: array of MPI_Status objects is not updated */
  return MPI_SUCCESS; 
}

//-------------------------------------------------------------------------------------------------------------------------


 /* local implementation */
int MPIX_Testany(int count, MPIX_Request array_of_requests[], int *index, int *flag, MPI_Status *status){
  
	//for each MPIX_request in provided array
	for(int i=0; i<count; i++){
	   	assert(MPIX_Test(&array_of_requests[i], flag, MPI_STATUS_IGNORE) == MPI_SUCCESS);
	   	if(*flag == 1){
	   		*index = i;
	   		break;
	   	}
	}
  return MPI_SUCCESS;	
}


 /* local implementation */
int MPIX_Testsome(int incount, MPIX_Request array_of_requests[],
                 int *outcount, int array_of_indices[],
                 MPI_Status array_of_statuses[]){
  
  int flag=0;
  *outcount = 0;
  for(int i=0; i<incount; i++){
	  assert(MPIX_Test(&array_of_requests[i], &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS);
	  if (flag == 1){
		 array_of_indices[*outcount]=i;
		 *outcount = *outcount+1;
	  }
  }
  return MPI_SUCCESS; 
}

int MPIX_Request_free(MPIX_Request *request){
  
  int i;
  for (i = 0; i < request->size; i++) 
    assert(MPI_Request_free(&request->request[i]) == MPI_SUCCESS);
 
  free(request->request);

  return MPI_SUCCESS; 
}

