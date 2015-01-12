/**
 * tests des routines de myblas:
 *  ddot
 *  dgemm
 *  dscal
 *  dger
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
    printf("\t cblas_ddot interlaced...");
    double ref = cblas_ddot(VEC_SIZE,vec,2,vec+1,2);
    printf("ok\n");

    printf("\t myblas_ddot_seq interlaced...");
    double ddot = myblas_ddot_seq(VEC_SIZE,vec,2,vec+1,2);
    ASSERT_EQ(ddot, ref);
    printf("ok\n");

    printf("\t myblas_ddot_seq_inc interlaced...");
    double ddot_inc = myblas_ddot_seq_inc(VEC_SIZE,vec,2,vec+1,2);
    ASSERT_EQ(ddot_inc, ref);
    printf("ok\n");

    /* Tests avec des vecteurs contigus : a1,a2...,an,b1,b2...,bn*/

    //ddot de reference (librairie systeme
    printf("\t cblas_ddot contiguous...");
    double ref2 = cblas_ddot(VEC_SIZE,vec,1,vec+VEC_SIZE,1);
    printf("ok\n");

    printf("\t myblas_ddot_seq contiguous...");
    double ddot2 = myblas_ddot_seq(VEC_SIZE,vec,1,vec+VEC_SIZE,1);
    ASSERT_EQ(ddot2, ref2);
    printf("ok\n");

    printf("\t myblas_ddot_seq_inc contiguous...");
    double ddot_inc2 = myblas_ddot_seq_inc(VEC_SIZE,vec,1,vec+VEC_SIZE,1);
    ASSERT_EQ(ddot_inc2, ref2);
    printf("ok\n");

    free(vec);
}

void test_dgemm()
{
    printf("Testing dgemm:\n");

    int m = MAT_SIZE_M/2;
    int n = MAT_SIZE_N/2;
    int k = MAT_SIZE_K/2;
    int i;

    double *A = matrix_rand(MAT_SIZE_K, MAT_SIZE_M);
    double *B = matrix_rand(MAT_SIZE_K, MAT_SIZE_N);
    double *C1 = matrix_rand(MAT_SIZE_M, MAT_SIZE_N);
    double *C2 = matrix_rand(MAT_SIZE_M, MAT_SIZE_N);

    printf("\t cblas_dgemm...\t\t");
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
  M = 3;
  N = 2;
  lda = M;
  ldb = M;
  double *A;
  double *B;
  A = calloc(M*M,sizeof(double));
  B = calloc(M*N,sizeof(double));
  if(B == NULL){
    fprintf(stderr,"ARGH\n");
  }
  A[0] = 1;
  A[1] = 2;
  A[2] = 7;
  A[1 + 1*lda] = 1;
  A[2 + 1*lda] = 3;
  A[2 + 2*lda] = 1;
  B[0] = 8;
  B[1] = 1;
  B[2] = 1;
  B[3] = 1;
  B[4] = 1;
  B[5] = 1;
  B[6] = 1;
  B[7] = 1;
  B[8] = 1;
  printf("Testing dtrsm()...\n");
  fprintf(stderr,"\nA :\n");
  affiche(M, M, A, lda, stdout);
  fprintf(stderr,"\nB :\n");
  affiche(M, N, B, ldb, stdout);
  myblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, M, N, 1.0, A, lda, B, ldb);
  fprintf(stderr,"\nX :\n");
  affiche(M, N, B, ldb, stdout);
  fprintf(stderr,"ok\n");
  free(A);
  free(B);
}

void test_blas_tdp5(){
  
  int i;

  double *X = matrix_rand(MAT_SIZE_M , 1);
  double *Y = matrix_rand(1, MAT_SIZE_N);
  double *A = matrix_rand(MAT_SIZE_M,MAT_SIZE_N);
  double *X_test = matrix_alloc(MAT_SIZE_M,1);
  double *A_test = matrix_alloc(MAT_SIZE_M,MAT_SIZE_N);
  memcpy(X_test, X, MAT_SIZE_M*sizeof(double));
  tab_nullify(A_test,MAT_SIZE_M*MAT_SIZE_N);

  printf("Testing dscal:... ");  
  myblas_dscal(MAT_SIZE_M, 2.0, X, 1);
  cblas_dscal(MAT_SIZE_M, 2.0, X_test, 1);
  for(i=0;i<MAT_SIZE_M;i++)
        ASSERT_EQ(X[i],X_test[i]);
  printf("ok\n");

  printf("Testing dger:... ");
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
