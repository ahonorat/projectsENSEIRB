#ifndef MYLAPACK_H
#define MYLAPACK_H

#ifdef USE_MKL
#include <mkl_lapacke.h>
#else
#include <lapacke.h>
#endif

//#define min(x,a,b) do {x = a<b ? a : b;}  while (0)

lapack_int mylapack_dgetf2(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv);

//blocked subfunction for dgetrf
lapack_int b_mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv, int bsize);

lapack_int mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv);

lapack_int mylapack_dgesv(int matrix_layout, int n, int nrhs, double *a, int lda, int *ipiv, double *b, int ldb, int *info);

lapack_int p_mylapack_dgetrf(int matrix_layout, int m, int n, double *a, int lda, int *ipiv, int *info);

lapack_int p_mylapack_dgesv(int matrix_layout, int n, int nrhs, double *a, int lda, int *ipiv, double *b, int ldb, int *info);

#endif
