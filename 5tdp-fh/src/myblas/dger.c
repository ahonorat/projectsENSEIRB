#include <assert.h>

#include "myblas.h"



void myblas_dger(const MYBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda)
{
    assert(order == CblasColMajor);
    int i,j, inc_X = 0, inc_Y = 0;
    for(j=0;j<N;j++){
      inc_X = 0;
      for(i=0;i<M;i++){
	A[j*lda+i]+=alpha*X[inc_X]*Y[inc_Y];
	inc_X += incX;
      }
      inc_Y += incY;
    }
}
