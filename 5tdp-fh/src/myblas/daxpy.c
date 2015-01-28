#include "myblas.h"

void myblas_daxpy(int N, double alpha, const double *X, int incX, double *Y, int incY)
{
    int i;
    for(i=0;i<N;i++)
        Y[i*incY] += alpha*X[i*incX];
}
