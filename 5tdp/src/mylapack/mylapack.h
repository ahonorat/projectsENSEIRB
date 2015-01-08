#ifndef MYLAPACK_H
#define MYLAPACK_H

#ifdef USE_MKL
#include <mkl_lapacke.h>
#else
#include <lapacke.h>
#endif


lapack_int mylapack_dgetf2(int matrix_layout, int m, int n, double *a, int lda, int *ipiv);

//lapack_int mylapack_dtrsm(int matrix_layout, char *side, char *uplo, char *transa, char *diag, int m, int n, double *alpha, double *a, int lda, double *b, int ldb);

lapack_int mylapack_dgetrf(int matrix_layout, int m, int n, double *a, int lda, int *ipiv, int *info);

lapack_int mylapack_dgesv(int matrix_layout, int n, int nrhs, double *a, int lda, int *ipiv, double *b, int ldb, int *info);

//lapack_int p_mylapack_dtrsm(int matrix_layout, char *side, char *uplo, char *transa, char *diag, int m, int n, double *alpha, double *a, int lda, double *b, int ldb);

lapack_int p_mylapack_dgetrf(int matrix_layout, int m, int n, double *a, int lda, int *ipiv, int *info);

lapack_int p_mylapack_dgesv(int matrix_layout, int n, int nrhs, double *a, int lda, int *ipiv, double *b, int ldb, int *info);

#endif
