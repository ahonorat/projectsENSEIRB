#include <assert.h>
#include <stdio.h>

#include <mpi.h>

#include "../util/util.h"
#include "myblas.h"

void myblas_dtrsm(const MYBLAS_ORDER Order, const MYBLAS_SIDE Side,
                 const MYBLAS_UPLO Uplo, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_DIAG Diag, const int M, const int N,
                 const double alpha, const double *A, const int lda,
		  double *B, const int ldb){
  assert(alpha == 1.0);
  assert(Order == CblasColMajor);
  assert(TransA == CblasNoTrans);
  if (Side == CblasLeft){
    int row;
    //In LU, case L(k,k)U(k,j) = A(k,j) 
    if (Uplo == CblasLower){
      for(row = 0; row < M; ++row){
	myblas_dscal(N, 1.0/A[row+row*lda], B+row, ldb);
	myblas_dger(CblasColMajor, M-1-row, N, -1.0, A+row*lda+row+1, 1, B+row, ldb, B+row+1, ldb);
      }
    } 
    //In dgesv, case Ux=y 
    else if (Diag == CblasUnit){
      for(row = M - 1; row > 0; --row){
	myblas_dger(CblasColMajor, row, N, -1.0, A+row*lda, 1, B+row, ldb, B, ldb);
      }
    }
  }
  //In LU, case L(i,k)U(k,k) = A(i,k)
  else if (Side == CblasRight && Uplo == CblasUpper && Diag == CblasUnit){
    int col;
    for(col = 0; col < (N - 1); ++col){
      myblas_dger(CblasColMajor, M, N-1-col, -1.0, B+col*ldb, 1, A+(col+1)*lda+col, lda, B+(col+1)*ldb, ldb);
    }
  } else {
    assert(0);
  }
}

