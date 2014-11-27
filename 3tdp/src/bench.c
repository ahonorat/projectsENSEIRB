#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>

#include "fox_mult/mult.h"
#include "util/proc.h"
#include "util/matrix.h"

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

int main(int argc, char** argv){

  struct matrix A;
  struct matrix B;
  struct matrix C;

  struct matrix local_A;
  struct matrix local_B;
  struct matrix local_C;

  struct grid grid;
  MPI_Comm comm;
  int nb_proc_row;
  int nb_proc_tot;
  int rank;
  int nb_in_block;

  int i, mat_size, nb_iter;
  struct timeval tv1, tv2;

  if (argc != 3) {
    MPI_Finalize();
    return EXIT_FAILURE;
  } else {
    mat_size = atoi(argv[1]);
    nb_iter = atoi(argv[2]);
  }

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc_tot);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (EXIT_FAILURE == compute_communicator(nb_proc_tot,&nb_proc_row,&comm,&rank)){
    MPI_Finalize();
    return EXIT_FAILURE;
  }
    
  mult_fox_mpi_init(nb_proc_row, &comm, &grid, rank);

  if (rank == 0){
    create_random_matrix(&A, mat_size, nb_proc_row);
    create_random_matrix(&B, mat_size, nb_proc_row);
    create_random_matrix(&C, mat_size, nb_proc_row);
    nb_in_block = A.length/nb_proc_row;
  }

  MPI_Bcast(&nb_in_block, 1, MPI_INT, 0, comm);

  create_random_matrix(&local_A, nb_in_block, 1);
  create_random_matrix(&local_B, nb_in_block, 1);
  create_random_matrix(&local_C, nb_in_block, 1);

  gettimeofday(&tv1, NULL);
  for(i = 0; i< nb_iter; i++){
    if (rank == 0){
      randomize_matrix(&A, mat_size);
      randomize_matrix(&B, mat_size);
    }
    matrix_placement_proc(nb_proc_row, nb_in_block, &comm, A.tab, local_A.tab, SCATTER);
    matrix_placement_proc(nb_proc_row, nb_in_block, &comm, B.tab, local_B.tab, SCATTER);
    mult_fox_mpi(nb_in_block, local_A.tab, local_B.tab, local_C.tab, &grid, comm);
    matrix_placement_proc(nb_proc_row, nb_in_block, &comm, local_C.tab, C.tab, GATHER);
  }
  gettimeofday(&tv2, NULL);


  free(local_A.tab);
  free(local_B.tab);
  free(local_C.tab);

  if (rank == 0){
    double time =(double)(tv2.tv_sec - tv1.tv_sec)*1000.0+ (double)(tv2.tv_usec - tv1.tv_usec)/1000.0;
    printf("%d\t%f\n",A.length,time/(double)nb_iter);//time in ms

    free(A.tab);
    free(B.tab);
    free(C.tab);
  }

  mult_fox_mpi_finalize(&grid);
  MPI_Comm_free(&comm);
  MPI_Finalize();

  return EXIT_SUCCESS;
}
