#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <assert.h>

#include "fox_mult/mult.h"
#include "util/proc.h"
#include "util/matrix.h"
#include "util/cblas_mkl.h"

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define ABS(a) ((a)>0?(a):-(a))
#define RELATIVE_ERROR 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>RELATIVE_ERROR){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); break;}

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

  if (argc < 2) {
    mat_size = DEFAULT_MAT_SIZE;
  } else {
    mat_size = atoi(argv[1]);
  }

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc_tot);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // initialization of the grid communicator
  if (EXIT_FAILURE == compute_communicator(nb_proc_tot,&nb_proc_row,&comm,&rank)){
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  // initialization of the proc localization in the grid   
  mult_fox_mpi_init(nb_proc_row, &comm, &grid, rank);

  // initialization of global matrix
  if (rank == 0){
    printf("Random matrix size (= nb_rows = nb_cols) : %d\n", mat_size);
    create_random_matrix(&A, mat_size, nb_proc_row);
    create_random_matrix(&B, mat_size, nb_proc_row);
    create_random_matrix(&C, mat_size, nb_proc_row);
    create_random_matrix(&D, mat_size, nb_proc_row);
    nb_in_block = A.length/nb_proc_row;
    mat_size = A.length;
    // computation by BLAS lib, to compare the result with
    for (i = 0; i<mat_size*mat_size; i++) D.tab[i] = 0.;
    cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,mat_size,mat_size,mat_size,1.0,A.tab,mat_size,B.tab,mat_size,1.0,D.tab,mat_size);
  }

  // broadcast of the number of row/col in a block
  MPI_Bcast(&nb_in_block, 1, MPI_INT, 0, comm);

  // initialization of local matrix
  create_random_matrix(&local_A, nb_in_block, 1);
  create_random_matrix(&local_B, nb_in_block, 1);
  create_random_matrix(&local_C, nb_in_block, 1);

  // core of the problem: scatter, fox mult and then gather
  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, A.tab, local_A.tab, SCATTER);
  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, B.tab, local_B.tab, SCATTER);
  
  mult_fox_mpi(nb_in_block, local_A.tab, local_B.tab, local_C.tab, &grid, comm);

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, local_C.tab, C.tab, GATHER);

  // each proc print his local matrix result
  int proc;
  int coords[2];
  for (proc=0; proc<nb_proc_row*nb_proc_row; proc++) {
    if (proc == rank) {
      MPI_Cart_coords(comm, rank, 2, coords);
      printf("Local matrix C, process: %d, coords: %d %d\n", rank, coords[0], coords[1]);
      print_matrix(&local_C, stdout);
    }
    MPI_Barrier(comm);
  }

  // all frees
  free(local_A.tab);
  free(local_B.tab);
  free(local_C.tab);

  if (rank == 0){
    for (i = 0; i<(C.length*C.length); i++){
      ASSERT_EQ(C.tab[i],D.tab[i]);
    }
    if (i == (C.length*C.length))
      printf("Matrix result is okay (comparison with BLAS library) (err < "STR(RELATIVE_ERROR)").\n");
    else
      printf("Matrix result is bad (at least one value is bad)");
    free(D.tab);
    printf("Matrix C result :\n");
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
