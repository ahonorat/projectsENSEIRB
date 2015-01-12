/**
 * tests des routines de myblas:
 *  ddot
 *  dgemm
 *  dscal
 *  dger
 *  dtrsm
 * Les tests sont des tests comparatif avec une librairie systeme (mkl, cblas...)
 **/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "myblas/myblas.h"
#include "util/util.h"

#define VEC_SIZE 50
#define ABS(a) ((a)>0?(a):-(a))

//assert Erreur relative < 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>0.000001){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define MAT_SIZE_M 20
#define MAT_SIZE_N 30
#define MAT_SIZE_K 10

void test_ddot()
{
    printf("Testing ddot:\n");
    double *vec = vector_rand(2*VEC_SIZE);

    /* Tests avec des vecteurs interlacés : a1,b1,a2,b2...*/

    //ddot de reference (librairie systeme
    printf("\t cblas_ddot interlaced...\t");
    double ref = cblas_ddot(VEC_SIZE,vec,2,vec+1,2);
    printf("ok\n");

    printf("\t myblas_ddot_seq interlaced...\t");
    double ddot = myblas_ddot_seq(VEC_SIZE,vec,2,vec+1,2);
    ASSERT_EQ(ddot, ref);
    printf("ok\n");

    printf("\t myblas_ddot_seq_inc interlaced...\t");
    double ddot_inc = myblas_ddot_seq_inc(VEC_SIZE,vec,2,vec+1,2);
    ASSERT_EQ(ddot_inc, ref);
    printf("ok\n");

    /* Tests avec des vecteurs contigus : a1,a2...,an,b1,b2...,bn*/

    //ddot de reference (librairie systeme
    printf("\t cblas_ddot contiguous...\t");
    double ref2 = cblas_ddot(VEC_SIZE,vec,1,vec+VEC_SIZE,1);
    printf("ok\n");

    printf("\t myblas_ddot_seq contiguous...\t");
    double ddot2 = myblas_ddot_seq(VEC_SIZE,vec,1,vec+VEC_SIZE,1);
    ASSERT_EQ(ddot2, ref2);
    printf("ok\n");

    printf("\t myblas_ddot_seq_inc contiguous...\t");
    double ddot_inc2 = myblas_ddot_seq_inc(VEC_SIZE,vec,1,vec+VEC_SIZE,1);
    ASSERT_EQ(ddot_inc2, ref2);
    printf("ok\n");

    free(vec);
}

void test_dgemm()
{
    printf("Testing dgemm:\n");

    int m = MAT_SIZE_M;
    int n = MAT_SIZE_N;
    int k = MAT_SIZE_K;
    int i;

    double *A = matrix_rand(MAT_SIZE_K, MAT_SIZE_M);
    double *B = matrix_rand(MAT_SIZE_K, MAT_SIZE_N);
    double *C1 = matrix_rand(MAT_SIZE_M, MAT_SIZE_N);
    double *C2 = matrix_rand(MAT_SIZE_M, MAT_SIZE_N);

    printf("\t cblas_dgemm...\t");
    tab_nullify(C1,MAT_SIZE_M*MAT_SIZE_N);
    cblas_dgemm(CblasColMajor,CblasTrans,CblasNoTrans,m,n,k,0.2,A,MAT_SIZE_K,B,MAT_SIZE_K,1.0,C1,MAT_SIZE_M);
    printf("ok\n");

    printf("\t myblas_dgemm_jik...\t");
    tab_nullify(C2,MAT_SIZE_M*MAT_SIZE_N);
    myblas_dgemm_jik(CblasColMajor,CblasTrans,CblasNoTrans,m,n,k,0.2,A,MAT_SIZE_K,B,MAT_SIZE_K,1.0,C2,MAT_SIZE_M);
    for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
        ASSERT_EQ(C1[i],C2[i]);
    printf("ok\n");

    printf("\t myblas_dgemm_bloc...\t");
    tab_nullify(C2,MAT_SIZE_M*MAT_SIZE_N);
    myblas_dgemm_bloc(CblasColMajor,CblasTrans,CblasNoTrans,m,n,k,0.2,A,MAT_SIZE_K,B,MAT_SIZE_K,1.0,C2,MAT_SIZE_M);
    for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
        ASSERT_EQ(C1[i],C2[i]);
    printf("ok\n");

    printf("\t myblas_dgemm_bloc_parallel...\t");
    tab_nullify(C2,MAT_SIZE_M*MAT_SIZE_N);
    myblas_dgemm_bloc_parallel(CblasColMajor,CblasTrans,CblasNoTrans,m,n,k,0.2,A,MAT_SIZE_K,B,MAT_SIZE_K,1.0,C2,MAT_SIZE_M);
    for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
        ASSERT_EQ(C1[i],C2[i]);
    printf("ok\n");

    free(A);
    free(B);
    free(C1);
    free(C2);
}

void test_dtrsm(){
  int M,N,lda,ldb;
  int i;
  M = 3; N = 2;
  lda = M; ldb = M;
  double *L; double *U;
  double *B1; double *B1_test;
  double *B2; double *B2_test;
  double *B3; double *B3_test;
  L = matrix_alloc(M,M);
  U = matrix_alloc(M,M);
  B1 = matrix_rand(M,N);
  B1_test = matrix_alloc(M,N);
  memcpy(B1_test, B1, M*N*sizeof(double));
  B2 = matrix_rand(M,N);
  B2_test = matrix_alloc(M,N);
  memcpy(B2_test, B2, M*N*sizeof(double));
  B3 = matrix_rand(M,N);
  B3_test = matrix_alloc(M,N);
  memcpy(B3_test, B3, M*N*sizeof(double));

  L[0] = 5; L[1 + 1*lda] = 1;
  L[1] = 2; L[2 + 1*lda] = 3;
  L[2] = 7; L[2 + 2*lda] = 4;
  U[9] = 1; U[1 + 1*lda] = 1;
  U[8] = 2; U[0 + 1*lda] = 3;
  U[7] = 7; U[0 + 0*lda] = 1;

  printf("Testing dtrsm:\n");

  printf("\t myblas_dtrsm(LL)...\t");
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, M, N, 1.0, L, lda, B1, ldb);
  cblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, M, N, 1.0, L, lda, B1_test, ldb);
  for(i=0;i<N*M;i++)
        ASSERT_EQ(B1[i],B1_test[i]);
  printf("ok\n");
  printf("\t myblas_dtrsm(LUU)...\t");
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasUnit, M, N, 1.0, U, lda, B2, ldb);
  cblas_dtrsm(CblasColMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasUnit, M, N, 1.0, U, lda, B2_test, ldb);
  for(i=0;i<N*M;i++)
        ASSERT_EQ(B2[i],B2_test[i]);
  printf("ok\n");
  printf("\t myblas_dtrsm(RUU)...\t");
  myblas_dtrsm(CblasColMajor, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, M, N, 1.0, U, lda, B3, ldb);
  cblas_dtrsm(CblasColMajor, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, M, N, 1.0, U, lda, B3_test, ldb);
  for(i=0;i<N*M;i++)
        ASSERT_EQ(B3[i],B3_test[i]);
  printf("ok\n");

  free(L); free(U);
  free(B1); free(B1_test);
  free(B2); free(B2_test);
  free(B3); free(B3_test);
}

void test_blas_tdp5(){
  
  int i;
  double *X = matrix_rand(MAT_SIZE_M , 1);
  double *Y = matrix_rand(1, MAT_SIZE_N);
  double *A = matrix_rand(MAT_SIZE_M,MAT_SIZE_N);
  double *X_test = matrix_alloc(MAT_SIZE_M,1);
  double *A_test = matrix_alloc(MAT_SIZE_M,MAT_SIZE_N);
  memcpy(X_test, X, MAT_SIZE_M*sizeof(double));
  memcpy(A_test, A, MAT_SIZE_M*MAT_SIZE_N*sizeof(double));

  printf("Testing dscal:...\t");  
  myblas_dscal(MAT_SIZE_M, 2.0, X, 1);
  cblas_dscal(MAT_SIZE_M, 2.0, X_test, 1);
  for(i=0;i<MAT_SIZE_M;i++)
        ASSERT_EQ(X[i],X_test[i]);
  printf("ok\n");

  printf("Testing dger:...\t");
  myblas_dger(CblasColMajor, MAT_SIZE_M, MAT_SIZE_N, 1.0, X, 1, Y, 1, A, MAT_SIZE_M);
  cblas_dger(CblasColMajor, MAT_SIZE_M, MAT_SIZE_N, 1.0, X, 1, Y, 1, A_test, MAT_SIZE_M);
  for(i=0;i<MAT_SIZE_M*MAT_SIZE_N;i++)
        ASSERT_EQ(A[i],A_test[i]);
  printf("ok\n");

  free(X);
  free(Y);
  free(A);
  free(X_test);
  free(A_test);

}


int main()
{
    test_ddot();
    test_dgemm();
    test_dtrsm();
    test_blas_tdp5();
    return 0;
}
