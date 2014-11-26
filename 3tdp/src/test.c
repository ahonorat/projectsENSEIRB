#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <assert.h>

#include "fox_mult/mult.h"
#include "util/proc.h"
#include "util/matrix.h"

#define FILENAMES_LEN 100

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define ABS(a) ((a)>0?(a):-(a))
#define RELATIVE_ERROR 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>RELATIVE_ERROR){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define DEFAULT_MAT_SIZE 4

int main(int argc, char** argv){

  struct matrix A;
  struct matrix B;
  struct matrix C;
  struct matrix D;

  struct matrix local_A;
  struct matrix local_B;
  struct matrix local_C;

  struct grid grid;
  MPI_Comm comm;
  int nb_proc_row;
  int nb_proc_tot;
  int rank;
  int nb_in_block;

  int i, mat_size;
  char a_filename[FILENAMES_LEN];
  char b_filename[FILENAMES_LEN];
  char d_filename[FILENAMES_LEN];

  a_filename[0] = '\0';
  b_filename[0] = '\0';
  d_filename[0] = '\0';

  if (argc < 2) {
    mat_size = DEFAULT_MAT_SIZE;
  } else {
    mat_size = atoi(argv[1]);
  }

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc_tot);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  compute_communicator(nb_proc_tot,&nb_proc_row,&comm,&rank);
  mult_fox_mpi_init(nb_proc_row, &comm, &grid, rank);

  if (rank == 0){
    printf("Random matrix size (= nb_rows = nb_cols) : %d\n", mat_size);
    if (a_filename[0] == '\0'){
      create_random_matrix(&A, mat_size, nb_proc_row);
    }
    if (b_filename[0] == '\0'){
      create_random_matrix(&B, mat_size, nb_proc_row);
    }
    create_random_matrix(&C, mat_size, nb_proc_row);
    assert(A.length == B.length);
    if (d_filename[0] != '\0'){
      assert(A.length == D.length);
    }
    nb_in_block = A.length/nb_proc_row;
  }

  MPI_Bcast(&nb_in_block, 1, MPI_INT, 0, comm);

  create_random_matrix(&local_A, nb_in_block, 1);
  create_random_matrix(&local_B, nb_in_block, 1);
  create_random_matrix(&local_C, nb_in_block, 1);

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, A.tab, local_A.tab, SCATTER);
  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, B.tab, local_B.tab, SCATTER);
  
  mult_fox_mpi(nb_in_block, local_A.tab, local_B.tab, local_C.tab, &grid, comm);

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, local_C.tab, C.tab, GATHER);

  int proc;
  for (proc=0; proc<nb_proc_row*nb_proc_row; proc++) {
    if (proc == rank) {
      printf("Process : %d, %d\n", rank, local_C.length);
      print_matrix(&local_C, stdout);
    }
    MPI_Barrier(comm);
  }

  //recompose matrix
  free(local_A.tab);
  free(local_B.tab);
  free(local_C.tab);

  if (rank == 0){
    if (d_filename[0] != '\0'){
      for (i = 0; i<(C.length*C.length); i++){
	ASSERT_EQ(C.tab[i],D.tab[i]);
      }
      printf("Matrix result is okay ... (err < "STR(RELATIVE_ERROR)").\n");
      free(D.tab);
    }
    print_matrix(&C, stdout);

    free(A.tab);
    free(B.tab);
    free(C.tab);
  }

  mult_fox_mpi_finalize(&grid);
  MPI_Comm_free(&comm);
  MPI_Finalize();

  return EXIT_SUCCESS;
}
