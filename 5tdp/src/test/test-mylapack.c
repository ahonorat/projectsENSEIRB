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

#define MIN(a,b) (a<b)?a:b

//assert Erreur relative < 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>0.000001){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define MAT_SIZE_M 40
#define MAT_SIZE_N 50
#define MAT_SIZE_K 10
#define BLOCK_SIZE 30

void test_dget_(){
  
  int i, min = (MAT_SIZE_M < MAT_SIZE_N)?MAT_SIZE_M:MAT_SIZE_N;
  int ipiv[MAT_SIZE_M]; // i.e. min (m,n)
  
  double *A = matrix_rand(MAT_SIZE_M , MAT_SIZE_N);
  double* L = matrix_alloc(MAT_SIZE_M, min);
  double* U = matrix_alloc(min, MAT_SIZE_N);
  double* C = matrix_alloc(MAT_SIZE_M, MAT_SIZE_N);
  memcpy(C, A, sizeof(double)*MAT_SIZE_M*MAT_SIZE_N);

  printf("Testing mylapack_dgetf2...");
  mylapack_dgetf2(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, ipiv);
  matrix_AtoLU(MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, L, MAT_SIZE_M, U, min);
  cblas_dgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,
	      MAT_SIZE_M,MAT_SIZE_N,min,
	      1.0,L,MAT_SIZE_M,
	      U,min,
              0.0,A,MAT_SIZE_M);
  for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
    ASSERT_EQ(A[i],C[i]);
  printf("\tok\n");
  
  free(A);
  free(L);
  free(U);
  free(C);
}

void test_dgetrf(){
  int m = 100;
  int n = 95;
  int bsize = 30;
  int min_mn = MIN(m,n);
  double *A = matrix_rand(m,n);
  double *C = matrix_alloc(m,n);
  double *L = matrix_alloc(m,min_mn);
  double *U = matrix_alloc(min_mn,n);
  int ipiv[m];
  int i;
  memcpy(C, A, m*n*sizeof(double));
  printf("Testing mylapack_dgetrf...");
  b_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, A, m, ipiv, bsize);
  matrix_AtoLU(m, n, A, m, L, m, U, min_mn);
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, min_mn, 1.0, L, m, U, min_mn, 0.0, A, m);
  for(i = 0; i < m*n; ++i){
    ASSERT_EQ(A[i], C[i]);
  }
  printf("\tok\n");
  free(A);
  free(C);
  free(L);
  free(U);
}

int main(){
  test_dget_();
  test_dgetrf();
  return 0;
}
