#include <assert.h>

#include <mpi.h>

#include "mylapack.h"
#include "../myblas/myblas.h"
#include "../util/util.h"

lapack_int mylapack_dgesv(int matrix_order, lapack_int n, lapack_int nrhs, double *a, lapack_int lda, lapack_int *ipiv, double *b, lapack_int ldb){
  assert(matrix_order == LAPACK_COL_MAJOR);
  mylapack_dgetrf(matrix_order, n, n, a, lda, ipiv);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, n, nrhs, 1.0, a, lda, b, ldb);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasUnit, n, nrhs, 1.0, a, lda, b, ldb);

  return 0;
}

lapack_int p_mylapack_dgesv(int matrix_order, lapack_int n, lapack_int nrhs, double *a, lapack_int lda, lapack_int *ipiv, double *b, lapack_int ldb){
  assert(matrix_order == LAPACK_COL_MAJOR);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int nb_cols = nb_col(size, BLOCK_SIZE, n, rank);
  double *a_local = matrix_alloc(n, nb_cols*BLOCK_SIZE);
  split_matrix(n, n, a, a_local, BLOCK_SIZE, SCATTER);
  p_mylapack_dgetrf(matrix_order, n, n, a_local, lda, ipiv);
  split_matrix(n, n, a, a_local, BLOCK_SIZE, GATHER);
  free(a_local);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, n, nrhs, 1.0, a, lda, b, ldb);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasUnit, n, nrhs, 1.0, a, lda, b, ldb);

  return 0;
}
