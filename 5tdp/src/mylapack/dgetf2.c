#include "mylapack.h"

//TODO : gérer le cas où l'élément diagonal est nul en échangeant la ligne avec une ligne où l'élément de colonne k n'est pas nul
//TODO : gérer le cas où M!=N
//Column major version
void mylapack_dgetf2(int * m, int * n, double * a, int * lda, int * ipiv, int * info){
  int k,j;
  for(k=0; k < N-1; ++k){
    myblas_dscal(N-k, 1/a[j + j*lda], a+(j+1)+j*lda, 1);
    myblas_dger(CblasColMajor, *m-j , *n-j, -1, a+(j+1)+(j*lda), 1, a+j+(j+1)*lda, lda, a+(j+1)+(j+1)*lda, lda);
  }
}
