/**
 * tests des routines de mylapack:
 *  dgetf2
 *  ...
 * Les tests sont des tests comparatif avec une librairie systeme (mkl, cblas...)
 **/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mylapack/mylapack.h"
#include "util/util.h"

#define ABS(a) ((a)>0?(a):-(a))

//assert Erreur relative < 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>0.000001){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define MAT_SIZE_M 5
#define MAT_SIZE_N 10
#define MAT_SIZE_K 10

void test_dget_(){

  int i;
  int ipiv[MAT_SIZE_M]; // i.e. min (m,n)
  for (i = 0; i < MAT_SIZE_M; ++i)
    ipiv[i] = i;

  double *A = matrix_rand(MAT_SIZE_M , MAT_SIZE_M);
  double* A_test = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  memcpy(A_test, A, sizeof(double)*MAT_SIZE_M*MAT_SIZE_M);

  printf("Testing dget_:\n");

  printf("\t mylapack_dgetf2...");
  mylapack_dgetf2(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_M, A, MAT_SIZE_M, ipiv);
  LAPACKE_dgetf2(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_M, A_test, MAT_SIZE_M, ipiv);
  affiche(MAT_SIZE_M, MAT_SIZE_M, A, MAT_SIZE_M, stdout);
  affiche(MAT_SIZE_M, MAT_SIZE_M, A_test, MAT_SIZE_M, stdout);
  for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
    ASSERT_EQ(A[i],A_test[i]);
  printf("ok\n");

  free(A);
  free(A_test);

}



int main(){
  test_dget_();
  return 0;
}
