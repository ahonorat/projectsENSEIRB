/*wrappers pour les routines ddot pour bench-generic (voir bench.h)*/

#include <stdio.h>
#include <stdlib.h>

#include "bench.h"
#include "myblas/myblas.h"
#include "util/perf.h"

#define N_ITER 5

long flop_dgemm(long i)
{
    return (2*i-1)*i*i;
}


void process_myblas_dgemm_bloc_parallel(void* data, int i)
{
    double** mat = data;

    myblas_dgemm_bloc_parallel(CblasColMajor,CblasTrans,CblasNoTrans,i,i,i,1.0,mat[0],BENCH_MAT_MAX_SIZE,mat[1],BENCH_MAT_MAX_SIZE,1.0,mat[2],BENCH_MAT_MAX_SIZE);
}


