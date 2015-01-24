#include <assert.h>

#include "mylapack.h"
#include "../myblas/myblas.h"

lapack_int mylapack_dgesv(int matrix_order, lapack_int n, lapack_int nrhs, double *a, lapack_int lda, lapack_int *ipiv, double *b, lapack_int ldb){
  assert(matrix_order == LAPACK_COL_MAJOR);
  mylapack_dgetrf(matrix_order, n, n, a, lda, ipiv);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, n, nrhs, 1.0, a, lda, b, ldb);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasUnit, n, nrhs, 1.0, a, lda, b, ldb);

  return 0;
}

lapack_int p_mylapack_dgesv(int matrix_order, lapack_int n, lapack_int nrhs, double *a, lapack_int lda, lapack_int *ipiv, double *b, lapack_int ldb){
  //TODO
  return 0;
}
