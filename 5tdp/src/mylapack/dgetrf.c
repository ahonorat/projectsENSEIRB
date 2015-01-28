#include <stdlib.h>
#include <assert.h>

#include <mpi.h>

#include "mylapack.h"
#include "../myblas/myblas.h"
#include "../util/util.h"

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
    //    current_bsize_n = bsize;
    for(i=k+1; i<nb_block_m; ++i){
      current_bsize_m = MIN(m-(i*bsize),bsize);
      //DTRSM : Solve L(i,k)U(k,k)=A(i,k)
      myblas_dtrsm(matrix_order, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, current_bsize_m, current_bsize_n, 1.0, &a[k*bsize + k*bsize*lda], lda, &a[i*bsize + k*bsize*lda], lda);
    }
    //    current_bsize_m = bsize;
    current_bsize_m = MIN(m-(k*bsize),bsize);
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
  int size, rank, nb_loc;
  double *dgetf_local = matrix_alloc(bsize,bsize);
  double *lastc_local = matrix_alloc(m,bsize);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Datatype blocktype;
  MPI_Datatype blocktype2; // resized in case of several are sent in the same time 
  MPI_Datatype blocktype3; // end of a column, maybe shorter
  MPI_Datatype blocktype4; // for the dgetf block
  MPI_Type_vector(bsize, bsize, m, MPI_DOUBLE, &blocktype);
  //  MPI_Type_create_resized(blocktype2, 0, bsize*sizeof(double), &blocktype); 
  MPI_Type_commit(&blocktype);
  MPI_Type_vector(bsize, m%bsize, m, MPI_DOUBLE, &blocktype3);
  MPI_Type_commit(&blocktype3);
  MPI_Type_contiguous(bsize*bsize, MPI_DOUBLE, &blocktype4);
  MPI_Type_commit(&blocktype4);
  int nb_block_m,nb_block_n,min_nb_block,current_bsize_m,current_bsize_n;
  int k = 0,i,j,p,q;
  nb_block_m = m%bsize == 0 ? m/bsize : (m/bsize)+1;
  nb_block_n = n%bsize == 0 ? n/bsize : (n/bsize)+1;
  min_nb_block = MIN(nb_block_m, nb_block_n);
  //Normally the diagonal block is always of size bsize*bsize, except in the last iteration of the very particular case where both dimensions of the original matrix aren't dividable by bsize
  MPI_Bcast(dgetf_local, 1, blocktype4, 0, MPI_COMM_WORLD);
  for(k=0; k < min_nb_block ; ++k){
    nb_loc = nb_local(k, rank, size);
    current_bsize_m = MIN(m-(k*bsize),bsize);
    current_bsize_n = MIN(n-(k*bsize),bsize);
   
    //LU(A(k,k))
    if (nb_loc){
      mylapack_dgetf2(matrix_order, current_bsize_m, current_bsize_n, &a[k*bsize + (nb_loc - 1)*bsize*lda], lda, &ipiv[k*bsize]);
      for(i = 0; i<current_bsize_m; ++i)
	for(j = 0; j<current_bsize_n; ++j)
	  dgetf_local[i+j*bsize] = a[k*bsize + (nb_loc - 1)*bsize*lda + i + j*lda];
    }
    MPI_Bcast(dgetf_local, 1, blocktype4, dest(k,size), MPI_COMM_WORLD);

    //    current_bsize_n = bsize;
    for(i=k+1; i<nb_block_m; ++i){
      current_bsize_m = MIN(m-(i*bsize),bsize);
      //DTRSM : Solve L(i,k)U(k,k)=A(i,k)
      if (nb_loc){
	myblas_dtrsm(matrix_order, CblasRight, CblasUpper, CblasNoTrans, CblasUnit, current_bsize_m, current_bsize_n, 1.0, dgetf_local, bsize, &a[i*bsize + (nb_loc - 1)*bsize*lda], lda);
	for(p = 0; p<current_bsize_m; ++p)
	  for(q = 0; q<current_bsize_n; ++q)
	    lastc_local[i*bsize + p + q*m] = a[i*bsize + (nb_loc - 1)*bsize*lda + p + q*lda];
      }
      if (current_bsize_m != bsize)
	MPI_Bcast(lastc_local+i*bsize, 1, blocktype3, dest(k,size), MPI_COMM_WORLD);
      else
	MPI_Bcast(lastc_local+i*bsize, 1, blocktype, dest(k,size), MPI_COMM_WORLD);
    }
    //    current_bsize_m = bsize;
    current_bsize_m = MIN(m-(k*bsize),bsize);
    for(j=k+1; j<nb_block_n; ++j){
      int nb_lcl = nb_local(j, rank, size);
      current_bsize_n = MIN(n-(j*bsize),bsize);
      //DTRSM : Solve L(k,k)U(k,j)=A(k,j)
      if (nb_lcl)
  	myblas_dtrsm(matrix_order, CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, current_bsize_m, current_bsize_n, 1.0, dgetf_local, bsize, &a[k*bsize + (nb_lcl - 1)*bsize*lda], lda);
    }
    for(i=k+1; i<nb_block_m; ++i){
      current_bsize_m = MIN(m-(i*bsize),bsize);
      for(j=k+1; j<nb_block_n; ++j){
	current_bsize_n = MIN(n-(j*bsize),bsize);
	int nb_lcl = nb_local(j, rank, size);
  	//DGEMM : A(i,j) = A(i,j) - L(i,k)*U(k,j)
	if (nb_lcl)
	  myblas_dgemm_jik(matrix_order, CblasNoTrans, CblasNoTrans, current_bsize_m, current_bsize_n, bsize, -1.0, &lastc_local[i*bsize], m, &a[k*bsize + (nb_lcl - 1)*bsize*lda], lda, 1.0, &a[i*bsize + (nb_lcl - 1)*bsize*lda], lda);
      }
    }
  }
  for(i=0; i<m; ++i)
      ipiv[i] = i;
  free(dgetf_local);
  free(lastc_local);
  return EXIT_SUCCESS;
}

lapack_int p_mylapack_dgetrf(int matrix_order, lapack_int m, lapack_int n, double *a, lapack_int lda, lapack_int *ipiv){
  return b_p_mylapack_dgetrf(LAPACK_COL_MAJOR, m, n, a, lda, ipiv, BLOCK_SIZE);
}
