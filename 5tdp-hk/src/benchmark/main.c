#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>
#include <sys/time.h>

#include "util/util.h"
#include "mylapack/mylapack.h"

#define N_ITER 4
#define INV_INC 5
#define MAT_SIZE 512
#define BENCH_B_MAX 500
#define BENCH_B_MIN 16

int main(){
  /* Benchmarks de dgetrf */

  int size = 1, rank;
  int i,j;
  MPI_Init(NULL, NULL);
  char filename[100];
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double * A[N_ITER];  
  int ipiv[MAT_SIZE];
  filename[0] = '\0';
  strcat(filename, "data_dgetrf");
  if (size == 1) {
    strcat(filename, "_seq");
    FILE* logfile = fopen(filename, "w+");
    fprintf(logfile, "#bsize\t ms \t(square matrix : %d)\n", MAT_SIZE);
    //On incrémente i de 25%
    for(i=BENCH_B_MIN; i <= BENCH_B_MAX; i+=i/INV_INC){
      printf("Bsize: %d\n", i);
      for (j = 0; j < N_ITER; ++j){
	A[j] = matrix_rand(MAT_SIZE, MAT_SIZE);
      }
      int time;
      struct timeval tv1, tv2;
      gettimeofday(&tv1, NULL);
      // le calcul est lancé plusieurs fois afin d'obtenir
      // des performances non faussées par d'autres processus
      for(j=0;j<N_ITER;++j){
	b_mylapack_dgetrf(LAPACK_COL_MAJOR, MAT_SIZE, MAT_SIZE, A[j], MAT_SIZE, ipiv, i);
      }
      //Ecriture du resultat dans le logfile et dans la console
      gettimeofday(&tv2, NULL);
      time = (tv2.tv_sec - tv1.tv_sec)*1000 + (tv2.tv_usec - tv1.tv_usec)/1000;
      fprintf(logfile,"%d\t%d\n",i,time);
      for (j = 0; j < N_ITER; ++j){
	free(A[j]);
      }
    }
    fclose(logfile);
    printf("Benchmark > %s\n",filename);
  } else { // idem pour plusieurs procs
    FILE* logfile;
    if (rank == 0){
      sprintf(filename+strlen(filename), "_%d", size);
      logfile = fopen(filename, "w+");
      fprintf(logfile, "#bsize\t ms \t(square matrix : %d)\n", MAT_SIZE);
    }
    for(i=BENCH_B_MIN; i <= BENCH_B_MAX; i+=i/INV_INC){
      int time;
      struct timeval tv1, tv2;
      double *A_local[N_ITER];
      int nb_cols = nb_col(size, i, MAT_SIZE, rank);
      for (j = 0; j < N_ITER; ++j){
	A_local[j] = matrix_rand(MAT_SIZE, nb_cols*i);
      }
      if (rank == 0){
	printf("Bsize: %d\n", i);
	gettimeofday(&tv1, NULL);
      }
      for (j = 0; j < N_ITER; ++j){
	b_p_mylapack_dgetrf(LAPACK_COL_MAJOR, MAT_SIZE, MAT_SIZE, A_local[j], MAT_SIZE, ipiv, i);
      }
      if (rank == 0){
	gettimeofday(&tv2, NULL);
        time = (tv2.tv_sec - tv1.tv_sec)*1000 + (tv2.tv_usec - tv1.tv_usec)/1000;
	fprintf(logfile,"%d\t%d\n",i,time);
      }
      for (j = 0; j < N_ITER; ++j){
	free(A_local[j]);
      }
    }
    if (rank == 0){
      fclose(logfile); 
      printf("Benchmark > %s\n",filename);
    }
  }
  MPI_Finalize();
  return 0;
}
