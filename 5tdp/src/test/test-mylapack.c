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

#include <mpi.h>

#include "myblas/myblas.h"
#include "mylapack/mylapack.h"
#include "util/util.h"

#define ABS(a) ((a)>0?(a):-(a))

#define MIN(a,b) (a<b)?a:b

//assert Erreur relative < 0.0001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>0.0001){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define MAT_SIZE_M 1024
#define MAT_SIZE_N 512
#define MAT_SIZE_K 10

void test_dgetf2(){
  
  int i, min = (MAT_SIZE_M < MAT_SIZE_N)?MAT_SIZE_M:MAT_SIZE_N;
  int ipiv[MAT_SIZE_M]; // i.e. min (m,n)
  
  double *A = matrix_rand(MAT_SIZE_M , MAT_SIZE_N);
  double* L = matrix_alloc(MAT_SIZE_M, min);
  double* U = matrix_alloc(min, MAT_SIZE_N);
  double* C = matrix_alloc(MAT_SIZE_M, MAT_SIZE_N);
  memcpy(C, A, sizeof(double)*MAT_SIZE_M*MAT_SIZE_N);

  printf("Testing mylapack_dgetf2...\t");
  mylapack_dgetf2(LAPACK_COL_MAJOR, MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, ipiv);
  matrix_AtoLU(MAT_SIZE_M, MAT_SIZE_N, A, MAT_SIZE_M, L, MAT_SIZE_M, U, min);
  cblas_dgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,
	      MAT_SIZE_M,MAT_SIZE_N,min,
	      1.0,L,MAT_SIZE_M,
	      U,min,
              0.0,A,MAT_SIZE_M);
  for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
    ASSERT_EQ(A[i],C[i]);
  printf("ok\n");
  
  free(A);
  free(L);
  free(U);
  free(C);
}

void test_dgetrf(){
  int m = MAT_SIZE_M;
  int n = MAT_SIZE_N;
  int bsize = BLOCK_SIZE;
  int min_mn = MIN(m,n);
  double *A = matrix_rand(m,n);
  double *C = matrix_alloc(m,n);
  double *L = matrix_alloc(m,min_mn);
  double *U = matrix_alloc(min_mn,n);
  int ipiv[m];
  int i;
  memcpy(C, A, m*n*sizeof(double));
  printf("Testing mylapack_dgetrf...\t");
  b_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, A, m, ipiv, bsize);
  matrix_AtoLU(m, n, A, m, L, m, U, min_mn);
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, min_mn, 1.0, L, m, U, min_mn, 0.0, A, m);
  for(i = 0; i < m*n; ++i){
    ASSERT_EQ(A[i], C[i]);
  }
  printf("ok\n");
  free(A);
  free(C);
  free(L);
  free(U);
}

void test_dgesv(){
  int m = MAT_SIZE_M;
  int n = MAT_SIZE_N;
  double *A = matrix_rand(m,m);
  double *A_test = matrix_alloc(m,m);
  double *B = matrix_rand(m,n);
  double *B_test = matrix_alloc(m,n);
  double *C = matrix_alloc(m,n);
  int ipiv[m];
  int i;
  memcpy(A_test, A, m*m*sizeof(double));
  memcpy(B_test, B, m*n*sizeof(double));
  printf("Testing mylapack_dgesv...\t");
  mylapack_dgesv(LAPACK_COL_MAJOR, m, n, A, m, ipiv, B, m);
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, m, 1.0, A_test, m, B, m, 0.0, C, m);
  for(i = 0; i < m*n; ++i){
    ASSERT_EQ(C[i], B_test[i]);
  }  
  printf("ok\n");
  free(A); free(A_test);
  free(B); free(B_test);
}

void test_p_dgetrf(){
  int m = MAT_SIZE_M;
  int n = MAT_SIZE_N;
  int bsize = BLOCK_SIZE;
  int min_mn = MIN(m,n);
  double *A; double *C; 
  double *L; double *U;
  int ipiv[m];
  int size, rank, i;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0){
    A = matrix_rand(m,n);
    C = matrix_alloc(m,n);
    L = matrix_alloc(m,min_mn);
    U = matrix_alloc(min_mn,n);
    memcpy(C, A, m*n*sizeof(double));
    printf("Testing p_mylapack_dgetrf...\t");
  }
  double *A_local;
  int nb_cols = nb_col(size, BLOCK_SIZE, n, rank);
  A_local = matrix_alloc(m, nb_cols*bsize);
  split_matrix(m, n, A, A_local, bsize, SCATTER);
  p_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, A_local, m, ipiv);
  split_matrix(m, n, A, A_local, bsize, GATHER);
  free(A_local);
  if (rank == 0){
    matrix_AtoLU(m, n, A, m, L, m, U, min_mn);
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, min_mn, 1.0, L, m, U, min_mn, 0.0, A, m);
    for(i = 0; i < m*n; ++i){
      ASSERT_EQ(A[i], C[i]);
    }
    printf("ok\n");
    free(A); free(C);
    free(L); free(U);
  }
}

void test_p_dgesv(){
  int m = MAT_SIZE_M;
  int n = MAT_SIZE_N;
  double *A = matrix_rand(m,m);
  double *A_test = matrix_alloc(m,m);
  double *B = matrix_rand(m,n);
  double *B_test = matrix_alloc(m,n);
  double *C = matrix_alloc(m,n);
  int ipiv[m];
  int i;
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  memcpy(A_test, A, m*m*sizeof(double));
  memcpy(B_test, B, m*n*sizeof(double));
  if (rank == 0)
    printf("Testing p_mylapack_dgesv...\t");
  p_mylapack_dgesv(LAPACK_COL_MAJOR, m, n, A, m, ipiv, B, m);
  if (rank == 0){
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, m, 1.0, A_test, m, B, m, 0.0, C, m);
    for(i = 0; i < m*n; ++i){
      ASSERT_EQ(C[i], B_test[i]);
    }  
    printf("ok\n");
  }
  free(A); free(A_test);
  free(B); free(B_test);
}


int main(){
  MPI_Init(NULL,NULL);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0){
    test_dgetf2();
    test_dgetrf();
    test_dgesv();
  }
  if (size > 1){
    test_p_dgetrf();
    test_p_dgesv();
  }
  MPI_Finalize();
  return 0;
}
