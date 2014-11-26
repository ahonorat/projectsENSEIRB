#include <stdlib.h>

#include "proc.h"

int compute_communicator(int nb_proc_tot, int* nb_proc_row, MPI_Comm* new_comm, int* rank){
  if (nb_proc_row == NULL || new_comm == NULL)
    return EXIT_FAILURE;
  register unsigned int op, res, one;  
  op = (unsigned int) nb_proc_tot;  
  res = 0;  
  /* "one" starts at the highest power of four <= than the argument. */  
  one = 1 << 30;  /* second-to-top bit set */  
  while (one > op) one >>= 2;  
  while (one != 0) {
    if (op >= res + one) {  
      op -= res + one;
      res += one << 1;  // <-- faster than 2 * one  
    }  
    res >>= 1;  
    one >>= 2;  
  }  
  *nb_proc_row = (int) res;

  const int dims[] =  {*nb_proc_row, *nb_proc_row};
  const int periods[] = {1, 1};

  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, new_comm);
  MPI_Comm_rank(*new_comm, rank);

  return EXIT_SUCCESS;
}

int matrix_placement_proc(int nb_proc_row, int nb_in_block, MPI_Comm* comm, double* sendbuf, double* rcvbuf, enum arrangement type){
  MPI_Datatype blocktype;
  MPI_Datatype blocktype2;  
  int ii, jj;

  MPI_Type_vector(nb_in_block, nb_in_block, nb_in_block*nb_proc_row, MPI_DOUBLE, &blocktype2);
  MPI_Type_create_resized(blocktype2, 0, sizeof(double), &blocktype);
  MPI_Type_commit(&blocktype);

  int disps[nb_proc_row*nb_proc_row];
  int counts[nb_proc_row*nb_proc_row];
  for (ii=0; ii<nb_proc_row; ii++) {
    for (jj=0; jj<nb_proc_row; jj++) {
      disps[ii*nb_proc_row+jj] = ii*nb_in_block*nb_in_block*nb_proc_row+jj*nb_in_block;
      counts [ii*nb_proc_row+jj] = 1;
    }
  }
  if (type == SCATTER) 
    MPI_Scatterv(sendbuf, counts, disps, blocktype, rcvbuf, nb_in_block*nb_in_block, MPI_DOUBLE, 0, *comm);
  else if (type == GATHER) 
    MPI_Gatherv(sendbuf, nb_in_block*nb_in_block, MPI_DOUBLE, rcvbuf, counts, disps, blocktype, 0, *comm);
  
  MPI_Type_free(&blocktype);
  
  if (type != GATHER && type != SCATTER)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
