/**
 * tests des routines de myblas:
 *  ddot
 *  dgemm
 * Les tests sont des tests comparatif avec une librairie systeme (mkl, cblas...)
 **/

#include <assert.h>
#include <stdlib.h>

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

void tab_nullify(double* tab, int N)
{
    int i;
    for(i=0;i<N;i++)
        tab[i]=0.0;
}

void test_dgemm()
{
    printf("Testing ddot:\n");

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

int main()
{
    test_ddot();
    test_dgemm();
    return 0;
}
