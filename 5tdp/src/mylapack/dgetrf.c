#include <stdlib.h>
#include <assert.h>

#include "mylapack.h"
#include "../myblas/myblas.h"

//Pas forcément le bon endroit pour déclarer cette fonction mais bon c'est temporaire
static int min(const int a, const int b){
  if(a<b) return a;
  else return b;
}

//ELOUAN : Version sans pivotage pour l'instant
lapack_int b_mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv, int bsize){
  assert(matrix_order == CblasColMajor);
  //nb_block_m : number of block by column
  //nb_block_n : number of block by row
  //current_bsize_m : number of rows of the current processed bloc
  //current_bsize_n : number of rows of the current processed bloc
  //diag_bsize : size of the current diagonal bloc (same size in both directions)
  int nb_block_m,nb_block_n,min_nb_block,current_bsize_m,current_bsize_n,k,i,j;
  nb_block_m = m%bsize == 0 ? m/bsize : (m/bsize)+1;
  nb_block_n = n%bsize == 0 ? n/bsize : (n/bsize)+1;
  min_nb_block = min(nb_block_m, nb_block_n);
  for(k=0; k<min_nb_block; ++k){
    //Normally the diagonal block is always of size bsize*bsize, except in the last iteration of the very particular case where both dimensions of the original matrix aren't dividable by bsize
    current_bsize_m = min(m-(k*bsize),bsize);
    current_bsize_n = min(n-(k*bsize),bsize);
    //LU(A(k,k))
    mylapack_dgetf2(matrix_order, current_bsize_m, current_bsize_n, &a[k*bsize + k*bsize*lda], lda, &ipiv[k]);
    for(i=0; i<nb_block_m-1; ++i){
      current_bsize_m = min(m-(i*bsize),bsize);
      current_bsize_n = bsize;
      //Solve L(i,k)U(k,k)=A(i,k)
      myblas_dtrsm(matrix_order, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[i*bsize + k*bsize*lda], lda);
    }
    for(j=0; j<nb_block_n-1; ++j){
      current_bsize_m = bsize;
      current_bsize_n = min(n-(j*bsize),bsize);
      //Solve L(k,k)U(k,j)=A(k,j)
      myblas_dtrsm(matrix_order, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[k*bsize + j*bsize*lda], lda);
    }
    for(j=k+1; j<nb_block_n; ++j){
      current_bsize_m = min(m-(j*bsize),bsize);
      for(i=k+1;i<nb_block_m;++i){
	current_bsize_n = min(n-(i*bsize),bsize);
	//A(i,j) = A(i,j) - L(i,k)*U(k,j)
	myblas_dgemm_jik(matrix_order, CblasNoTrans, CblasNoTrans, current_bsize_m, current_bsize_n, bsize, -1.0, &a[i*bsize + k*bsize*lda], lda, &a[k*bsize + j*bsize*lda], lda, 1.0, &a[i*bsize + j*bsize*lda], lda);
      }
    }
  }
  return EXIT_SUCCESS;
}

lapack_int mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double* a, lapack_int lda, lapack_int* ipiv){
  //TODO
  return EXIT_SUCCESS;
}

lapack_int p_mylapack_dgetrf(int matrix_layout, lapack_int m, lapack_int n, double *a, lapack_int lda, lapack_int *ipiv, lapack_int *info){
  //TODO
  return 0;
}
