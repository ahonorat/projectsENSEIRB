#include <assert.h>

#include "../myblas/myblas.h"
#include "mylapack.h"

//TODO : gérer le cas où l'élément diagonal est nul en échangeant la ligne avec une ligne où l'élément de colonne k n'est pas nul
//TODO : gérer le cas où M!=N
//Column major version
lapack_int mylapack_dgetf2(int matrix_layout, lapack_int m, lapack_int n, double *a, lapack_int lda, lapack_int *ipiv){
  assert(matrix_layout == LAPACK_COL_MAJOR);
  int k, min_dim;
  min_dim = (m < n)?m:n;
  for(k=0; k < (min_dim - 1); ++k){
    myblas_dscal(min_dim - k, 1.0 / a[k+k*lda], &a[(k+1)+(k+1)*lda], 1);
    myblas_dger(CblasColMajor, m-k , n-k, -1.0, &a[(k+1)+k*lda], 1, &a[k+(k+1)*lda], lda, &a[(k+1)+(k+1)*lda], lda);
  }
  return 0;
}
