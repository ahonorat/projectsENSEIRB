#ifndef MYLAPACK_H
#define MYLAPACK_H

#include "../myblas/myblas.h"

void mylapack_dgetf2(int * m, int * n, double * a, int * lda, int * ipiv, int * info);

int mylapack_dtrsm(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int * lda, double *b, int *ldb);

int mylapack_dgetrf(int *m, int *n, double *a, int * lda, int *ipiv, int *info);

int mylapack_dgesv(int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);

int p_mylapack_dtrsm(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int * lda, double *b, int *ldb);

int p_mylapack_dgetrf(int *m, int *n, double *a, int * lda, int *ipiv, int *info);

int p_mylapack_dgesv(int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);

#endif
