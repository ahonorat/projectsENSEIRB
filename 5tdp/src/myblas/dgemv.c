#include <assert.h>

#include "myblas.h"

#define coord_ta(i,j) (i)*lda+(j)
#define coord_b(i,j) (j)*ldb+(i)
#define coord_c(i,j) (j)*ldc+(i)

void myblas_dgemv(const MYBLAS_TRANSPOSE Trans, const int M, const int N, 
		  const double alpha, const double *A, const int lda, 
		  const double *X, const int incX, 
		  const double beta, double *Y, const int incY)
{
  assert(Trans == CblasTrans);

  int i,j;
    for(i = 0; i<M; i++)//parcours des lignes de tA
      {
	Y[i*incY] *= beta;
	for(j = 0; j<N; j++)//parcours des lignes de B
	  {
	    Y[i*incY] += alpha*A[coord_ta(i,j)]*X[j*incX];
	}
      }
}


