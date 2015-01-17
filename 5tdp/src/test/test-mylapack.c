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

#define MAT_SIZE_M 6
#define MAT_SIZE_N 6
#define MAT_SIZE_K 10
#define BLOCK_SIZE 3

void test_dget_(){

  int i, min = (MAT_SIZE_M < MAT_SIZE_N)?MAT_SIZE_M:MAT_SIZE_N;
  int ipiv[MAT_SIZE_M]; // i.e. min (m,n)

  double *A = matrix_rand(MAT_SIZE_M , MAT_SIZE_N);
  double* L = matrix_alloc(MAT_SIZE_M, min);
  double* U = matrix_alloc(min, MAT_SIZE_N);
  double* C = matrix_alloc(MAT_SIZE_M, MAT_SIZE_N);
  memcpy(C, A, sizeof(double)*MAT_SIZE_M*MAT_SIZE_N);

  printf("Testing dget_:\n");

  printf("\t mylapack_dgetf2...\n");
  //mylapack_dgetf2(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, ipiv);
  b_mylapack_dgetrf(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, ipiv, BLOCK_SIZE);
  matrix_AtoLU(MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, L, MAT_SIZE_M, U, min);
  cblas_dgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,
	      MAT_SIZE_M,MAT_SIZE_N,min,
	      1.0,L,MAT_SIZE_M,
	          U,min,
              0.0,A,MAT_SIZE_M);
  for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
    ASSERT_EQ(A[i],C[i]);
  printf("ok\n");

  /*
  printf("\t mylapack_dgetrf...\n");
  int m = 4;
  int n = 5;
  int bsize = 2;
  int min_mn = (m < n)?m:n;
  lapack_int* ipiv2;
  ipiv2 = malloc(m*sizeof(lapack_int));
  double *A2 = matrix_alloc(m , n);
  double *L2 = matrix_alloc(m, min_mn);
  double *U2 = matrix_alloc(min_mn, n);
  A2[0] = 1;  A2[4] = 2;   A2[8] = 2;  A2[12] = 2;  A2[16] = 2;
  A2[1] = 2;  A2[5] = 7;  A2[9] = 10; A2[13] = 10; A2[17] = 10;
  A2[2] = 4; A2[6] = 13; A2[10] = 14; A2[14] = 30; A2[18] = 30;
  A2[3] = 5; A2[7] = 14; A2[11] = 21; A2[15] = 26; A2[19] = 28;
  printf("before :\n");
  affiche(m,n,A2,m,stdout);
  //mylapack_dgetf2(LAPACK_COL_MAJOR, m, n, A2, m, ipiv2);
  b_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, A2, m, ipiv2, bsize);
  matrix_AtoLU(m, n, A2, m, L2, m, U2, min_mn);
  
  printf("\n L :\n");
  affiche(m,min_mn,L2,m,stdout);
  printf("\n U :\n");
  affiche(min_mn,n,U2,min_mn,stdout);
  printf("ok\n");

  free(A2);
  free(L2);
  free(U2);
  free(ipiv2);*/


  free(A);
  free(L);
  free(U);
  free(C);
}



int main(){
  test_dget_();
  return 0;
}
