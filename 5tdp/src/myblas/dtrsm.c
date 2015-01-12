#include <assert.h>
#include <stdio.h>

#include "myblas.h"
#include "../util/util.h"

void myblas_dtrsm(const MYBLAS_ORDER Order, const MYBLAS_SIDE Side, const MYBLAS_UPLO Uplo, const MYBLAS_TRANSPOSE TransA, const MYBLAS_DIAG Diag, const int M, const int N, const double alpha, const double *A, const int lda, double *B, const int ldb){
  assert(Order == CblasColMajor);
  //Case L(k,k)U(k,j) = A(k,j) 
  if(Side == CblasLeft && Uplo == CblasLower && TransA == CblasNoTrans && Diag == CblasUnit){
    int line;
    for(line=0; line<M; ++line){
      //It's useless to divide by A(i,i) because A has a unit diagonal
      //myblas_dscal(N, 1.0/A[line+line*lda], B+line, ldb);
      myblas_dger(CblasColMajor, M-line-1, N, -1.0, &A[(line+1)+line*lda], 1, &B[line], ldb, &B[line+1], ldb);
    }
  }
  //Case L(i,k)U(k,k) = A(i,k)
  else{

  }
}
