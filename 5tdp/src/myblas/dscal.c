#include "myblas.h"

void myblas_dscal(const int N, const double alpha, double *X, const int incX){
  int i, j;
  j = 0;
  for (i = 0; i < N; ++i){
    X[j] *= alpha;
    j += incX;
  }
}
