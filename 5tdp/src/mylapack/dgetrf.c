#include <stdlib.h>
#include <assert.h>

#include "mylapack.h"
#include "../myblas/myblas.h"

#define BLOCK_SIZE 10

#define MIN(a,b) (a<b)?a:b

lapack_int b_mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv, int bsize){
  assert(matrix_order == LAPACK_COL_MAJOR);
  //nb_block_m : number of block by column
  //nb_block_n : number of block by row
  //current_bsize_m : number of rows of the currently processed bloc
  //current_bsize_n : number of columns of the currently processed bloc
  int nb_block_m,nb_block_n,min_nb_block,current_bsize_m,current_bsize_n,k,i,j;
  nb_block_m = m%bsize == 0 ? m/bsize : (m/bsize)+1;
  nb_block_n = n%bsize == 0 ? n/bsize : (n/bsize)+1;
  min_nb_block = MIN(nb_block_m, nb_block_n);
  for(k=0; k<min_nb_block; ++k){
    //Normally the diagonal block is always of size bsize*bsize, except in the last iteration of the very particular case where both dimensions of the original matrix aren't dividable by bsize
    current_bsize_m = MIN(m-(k*bsize),bsize);
    current_bsize_n = MIN(n-(k*bsize),bsize);
    //LU(A(k,k))
    mylapack_dgetf2(matrix_order, current_bsize_m, current_bsize_n, &a[k*bsize + k*bsize*lda], lda, &ipiv[k*bsize]);
    current_bsize_n = bsize;
    for(i=k+1; i<nb_block_m; ++i){
      current_bsize_m = MIN(m-(i*bsize),bsize);
      //DTRSM : Solve L(i,k)U(k,k)=A(i,k)
      myblas_dtrsm(matrix_order, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[i*bsize + k*bsize*lda], lda);
    }
    current_bsize_m = bsize;
    for(j=k+1; j<nb_block_n; ++j){
      current_bsize_n = MIN(n-(j*bsize),bsize);
      //DTRSM : Solve L(k,k)U(k,j)=A(k,j)
	myblas_dtrsm(matrix_order, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[k*bsize + j*bsize*lda], lda);
    }
    for(j=k+1; j<nb_block_n; ++j){
      current_bsize_n = MIN(n-(j*bsize),bsize);
      for(i=k+1;i<nb_block_m; ++i){
	current_bsize_m = MIN(m-(i*bsize),bsize);
	//DGEMM : A(i,j) = A(i,j) - L(i,k)*U(k,j)
	myblas_dgemm_jik(matrix_order, CblasNoTrans, CblasNoTrans, current_bsize_m, current_bsize_n, bsize, -1.0, &a[i*bsize + k*bsize*lda], lda, &a[k*bsize + j*bsize*lda], lda, 1.0, &a[i*bsize + j*bsize*lda], lda);
      }
    }
  }
  for(i=0; i<m; ++i)
      ipiv[i] = i;
  return EXIT_SUCCESS;
}

lapack_int mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv){
  return b_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, a, lda, ipiv, BLOCK_SIZE);
}

lapack_int b_p_mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double *a, lapack_int lda, lapack_int *ipiv, int bsize){
  assert(matrix_order == LAPACK_COL_MAJOR);
  //nb_block_m : number of block by column
  //nb_block_n : number of block by row
  //current_bsize_m : number of rows of the currently processed bloc
  //current_bsize_n : number of columns of the currently processed bloc
  int nb_block_m,nb_block_n,min_nb_block,current_bsize_m,current_bsize_n,k,i,j;
  nb_block_m = m%bsize == 0 ? m/bsize : (m/bsize)+1;
  nb_block_n = n%bsize == 0 ? n/bsize : (n/bsize)+1;
  min_nb_block = MIN(nb_block_m, nb_block_n);
  for(k=0; k<min_nb_block; ++k){
    //Normally the diagonal block is always of size bsize*bsize, except in the last iteration of the very particular case where both dimensions of the original matrix aren't dividable by bsize
    current_bsize_m = MIN(m-(k*bsize),bsize);
    current_bsize_n = MIN(n-(k*bsize),bsize);
    //LU(A(k,k))
    mylapack_dgetf2(matrix_order, current_bsize_m, current_bsize_n, &a[k*bsize + k*bsize*lda], lda, &ipiv[k*bsize]);
    current_bsize_n = bsize;
    for(i=k+1; i<nb_block_m; ++i){
      current_bsize_m = MIN(m-(i*bsize),bsize);
      //DTRSM : Solve L(i,k)U(k,k)=A(i,k)
      myblas_dtrsm(matrix_order, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[i*bsize + k*bsize*lda], lda);
    }
    current_bsize_m = bsize;
    for(j=k+1; j<nb_block_n; ++j){
      current_bsize_n = MIN(n-(j*bsize),bsize);
      //DTRSM : Solve L(k,k)U(k,j)=A(k,j)
	myblas_dtrsm(matrix_order, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[k*bsize + j*bsize*lda], lda);
    }
    for(j=k+1; j<nb_block_n; ++j){
      current_bsize_n = MIN(n-(j*bsize),bsize);
      for(i=k+1;i<nb_block_m; ++i){
	current_bsize_m = MIN(m-(i*bsize),bsize);
	//DGEMM : A(i,j) = A(i,j) - L(i,k)*U(k,j)
	myblas_dgemm_jik(matrix_order, CblasNoTrans, CblasNoTrans, current_bsize_m, current_bsize_n, bsize, -1.0, &a[i*bsize + k*bsize*lda], lda, &a[k*bsize + j*bsize*lda], lda, 1.0, &a[i*bsize + j*bsize*lda], lda);
      }
    }
  }
  for(i=0; i<m; ++i)
      ipiv[i] = i;
  return EXIT_SUCCESS;
}

lapack_int p_mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double *a, lapack_int lda, lapack_int *ipiv){
  return b_p_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, a, lda, ipiv, BLOCK_SIZE);
}
