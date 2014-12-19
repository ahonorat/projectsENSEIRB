#include <stdio.h>
#include <stdlib.h>

#include "util/perf.h"
#include "util/util.h"
#include "myblas/myblas.h"
#include "benchmark/bench.h"

int main(){
    /* Benchmarks de dgemm (voir les differents process_* dans bench.h pour voir leur comportement)*/
    double* data_dgemm[3];
    data_dgemm[0] = matrix_rand(BENCH_MAT_MAX_SIZE,BENCH_MAT_MAX_SIZE);
    data_dgemm[1] = matrix_rand(BENCH_MAT_MAX_SIZE,BENCH_MAT_MAX_SIZE);
    data_dgemm[2] = matrix_rand(BENCH_MAT_MAX_SIZE,BENCH_MAT_MAX_SIZE);
    bench_generic( "myblas_dgemm_bloc_parallel", BENCH_MAT_MAX_SIZE,
                    flop_dgemm,
                    process_myblas_dgemm_bloc_parallel,
                    data_dgemm);

    free(data_dgemm[0]);
    free(data_dgemm[1]);
    free(data_dgemm[2]);

    return 0;
}
