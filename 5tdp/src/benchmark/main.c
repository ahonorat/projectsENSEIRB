#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

#include "util/util.h"
#include "util/perf.h"
#include "mylapack/mylapack.h"

#define N_ITER 4
#define MAT_SIZE 1024
#define BENCH_B_MAX 512
#define BENCH_B_MIN 64

// flops for dgetrf
long flop(long i){
  
  return 0;
}

int main(){
  /* Benchmarks de dgetrf */

  int size = 1, rank;
  long i; int j;
  MPI_Init(NULL, NULL);
  char filename[100];
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double * A[N_ITER];  
  filename[0] = '\0';
  strcat(filename, "data_dgetrf");
  if (size == 1) {
    strcat(filename, "_seq");
    FILE* logfile = fopen(filename, "w+");
    fprintf(logfile, "#bsize\tMFlop/s\n");
    //On incrémente i de 25%
    for(i=BENCH_B_MIN; i <= BENCH_B_MAX; i+=i/4){
      printf("Bsize: %ld\n", i);
      for (j = 0; j < N_ITER; ++j){
	A[j] = matrix_rand(MAT_SIZE, MAT_SIZE);
      }
      int ipiv[MAT_SIZE];
      perf_t start,stop;
      perf(&start);
      // le calcul est lancé plusieurs fois afin d'obtenir
      // des performances non faussées par d'autres processus
      for(j=0;j<N_ITER;++j){
	b_mylapack_dgetrf(LAPACK_COL_MAJOR, MAT_SIZE, MAT_SIZE, A[j], MAT_SIZE, ipiv, i);
      }
      perf(&stop);
      perf_diff(&start, &stop);
      double mflops = perf_mflops(&stop, flop(i)*N_ITER);
      //Ecriture du resultat dans le logfile et dans la console
      fprintf(logfile,"%lu\t%f\n",i,mflops);
      for (j = 0; j < N_ITER; ++j){
	free(A[j]);
      }
    }
    fclose(logfile);
  } else {
    FILE* logfile;
    if (rank == 0){
      sprintf(filename+strlen(filename), "_%d", size);
      logfile = fopen(filename, "w+");
      fprintf(logfile, "#bsize\tMFlop/s\n");
	for (j = 0; j < N_ITER; ++j){
	  A[j] = matrix_rand(MAT_SIZE, MAT_SIZE);
	}
    }
    for(i=BENCH_B_MIN; i <= BENCH_B_MAX; i+=i/4){
      int ipiv[MAT_SIZE];
      perf_t start,stop;
      double *A_local[N_ITER];
      if (rank == 0){
	printf("Bsize: %ld\n", i);
	perf(&start);
      }
      int nb_cols = nb_col(size, i, MAT_SIZE, rank);
      for (j = 0; j < N_ITER; ++j){
	A_local[j] = matrix_alloc(MAT_SIZE, nb_cols*i);
	split_matrix(MAT_SIZE, MAT_SIZE, A[j], A_local[j], i, SCATTER);
      }
      for(j=0;j<N_ITER;++j){
	b_p_mylapack_dgetrf(LAPACK_COL_MAJOR, MAT_SIZE, MAT_SIZE, A[j], MAT_SIZE, ipiv, i);
      }
      if (rank == 0){
	perf(&stop);
	perf_diff(&start, &stop);
	double mflops = perf_mflops(&stop, flop(i)*N_ITER);
	//Ecriture du resultat dans le logfile et dans la console
	fprintf(logfile,"%lu\t%f\n",i,mflops);
      }
      for (j = 0; j < N_ITER; ++j){
	free(A_local[j]);
      }
    }
    if (rank == 0){
      for (j = 0; j < N_ITER; ++j){
	free(A[j]);
      }
      fclose(logfile); 
      printf("Benchmark > %s\n",filename);
    }
  }
  MPI_Finalize();
  return 0;
}
