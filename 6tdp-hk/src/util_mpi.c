#include <stdlib.h>

#include <mpi.h>

#include "util_mpi.h"

int compute_communicator(int nb_proc_tot, int* nb_proc_row, MPI_Comm* new_comm, int* rank){
  if (nb_proc_row == NULL || new_comm == NULL)
    return EXIT_FAILURE;

  // classical algo to compute quickly integer square root
  // this square root will be the amout of process in each dimension of the matrix
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

  // creation of the communicator for the grid
  const int dims[] =  {*nb_proc_row, *nb_proc_row};
  const int periods[] = {1, 1};

  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, new_comm);
  if (*new_comm != MPI_COMM_NULL)
    MPI_Comm_rank(*new_comm, rank);
  else 
    return EXIT_FAILURE;
  // some process may be unnecessary (if nb_proc_tot isn't a integer power 2)
  // so they are stopped
  return EXIT_SUCCESS;
}

int matrix_placement_proc(int nb_proc_row, int nb_in_block, MPI_Comm* comm, double* sendbuf, double* rcvbuf, enum arrangement type){
  MPI_Datatype blocktype;
  MPI_Datatype blocktype2;  
  int ii, jj;


  // first you create the type representation for a matrix bloc associated to a process
  MPI_Type_vector(nb_in_block, nb_in_block, nb_in_block*nb_proc_row, MPI_INT, &blocktype2);
  MPI_Type_create_resized(blocktype2, 0, sizeof(int), &blocktype);
  MPI_Type_commit(&blocktype);

  int disps[nb_proc_row*nb_proc_row];
  int counts[nb_proc_row*nb_proc_row];
  for (ii=0; ii<nb_proc_row; ii++) {
    for (jj=0; jj<nb_proc_row; jj++) {
      disps[ii*nb_proc_row+jj] = ii*nb_in_block*nb_in_block*nb_proc_row+jj*nb_in_block;
      counts [ii*nb_proc_row+jj] = 1;
    }
  }

  // scatter or gather
  if (type == SCATTER) 
    MPI_Scatterv(sendbuf, counts, disps, blocktype, rcvbuf, nb_in_block*nb_in_block, MPI_INT, 0, *comm);
  else if (type == GATHER) 
    MPI_Gatherv(sendbuf, nb_in_block*nb_in_block, MPI_INT, rcvbuf, counts, disps, blocktype, 0, *comm);
  
  MPI_Type_free(&blocktype);
  
  if (type != GATHER && type != SCATTER)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int mpi_grid_init(MPI_Comm* comm, struct grid* grid, int rank){
  int coords[2];
  MPI_Cart_coords(*comm, rank, 2, coords);
  grid->rank_I = coords[0];
  grid->rank_J = coords[1];
  // determination of neighbours rank (above and under)
  int r;
  coords[0]++; 
  MPI_Cart_rank(*comm, coords, &r);
  grid->proc_under = r;
  coords[0] -= 2; 
  MPI_Cart_rank(*comm, coords, &r);
  grid->proc_above = r;
  coords[0] += 1;
  coords[1]++;
  MPI_Cart_rank(*comm, coords, &r);
  grid->proc_right = r;
  coords[1] -= 2;
  MPI_Cart_rank(*comm, coords, &r);
  grid->proc_left = r;

  return EXIT_SUCCESS;
}
