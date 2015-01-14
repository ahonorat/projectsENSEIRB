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

#include "myblas/myblas.h"
#include "mylapack/mylapack.h"
#include "util/util.h"

#define ABS(a) ((a)>0?(a):-(a))

//assert Erreur relative < 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>0.000001){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define MAT_SIZE_M 30
#define MAT_SIZE_N 10
#define MAT_SIZE_K 10

void test_dget_(){

  int i;
  int ipiv[MAT_SIZE_M]; // i.e. min (m,n)

  double *A = matrix_rand(MAT_SIZE_M , MAT_SIZE_M);
  double* L = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  double* U = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  double* C = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  memcpy(C, A, sizeof(double)*MAT_SIZE_M*MAT_SIZE_M);

  printf("Testing dget_:\n");

  printf("\t mylapack_dgetf2...\n");
  mylapack_dgetf2(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_M, A, MAT_SIZE_M, ipiv);
  matrix_AtoLU(MAT_SIZE_M, MAT_SIZE_M, A, MAT_SIZE_M, L, MAT_SIZE_M, U, MAT_SIZE_M);
  cblas_dgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,
	      MAT_SIZE_M,MAT_SIZE_M,MAT_SIZE_M,
	      1.0,L,MAT_SIZE_M,
	          U,MAT_SIZE_M,
              0.0,A,MAT_SIZE_M);
  for(i=0;i<MAT_SIZE_M*MAT_SIZE_M;i++)
    ASSERT_EQ(A[i],C[i]);
  printf("ok\n");


  /*
  printf("\t mylapack_dgetrf...\n");
  int m = 4;
  int n = 4;
  lapack_int* ipiv2;
  ipiv2 = malloc(m*sizeof(lapack_int));
  int j;
  double *A2 = matrix_rand(MAT_SIZE_M , MAT_SIZE_M);
  double* L2 = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  double* U2 = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  double* C2 = matrix_alloc(MAT_SIZE_M, MAT_SIZE_M);
  for(i=0;i<m;++i){
    for(j=0;j<m;++j){
      A2[i+m*j] = 1+i+m*j < m*n-(1+i+m*j) ? 1+i+m*j : m*n-(1+i+m*j);
    }
  }
  A2[n*m-1] = 1;
  printf("before :\n");
  affiche(m,n,A2,m,stdout);
  mylapack_dgetf2(LAPACK_COL_MAJOR, m, n, A2, m, ipiv2);
  //mylapack_dgetrf(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_M, );
  printf("\n after :\n");
  affiche(m,n,A2,m,stdout);
  printf("ok\n");
  free(A2);
  free(L2);
  free(U2);
  free(C2);
  free(ipiv2);
*/ 


  free(A);
  free(L);
  free(U);
  free(C);
}



int main(){
  test_dget_();
  return 0;
}
