#include <assert.h>

#include "myblas.h"



void myblas_dger(const MYBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda)
{
    assert(order == CblasColMajor);
    int i,j;
    for(j=0;j<N;j++)
        for(i=0;i<M;i++)
            A[j*lda+i]+=alpha*X[i*incX]*Y[j*incY];
}

void myblas_dger_bloc_parallel(const MYBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda)
{
    myblas_dgemm_bloc_parallel(order, CblasNoTrans,
                 CblasNoTrans, M, N,
                 1, alpha, X,
                 M, Y, 1,
                 1.0, A, lda);
}
