#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#include <mpi.h>

#include "util.h"

#define CHK(X) if((X)<=0) {perror(#X); exit(EXIT_FAILURE);}

double rand_double(){
  double res = (double) rand()/RAND_MAX;
  res *= 10;
  res = (rand()%2 == 0)?(res):(-res);
  return res;
}

double* vector_alloc(int n)
{
    double* vec;
    CHK(vec = malloc(sizeof(double)*n));
    return vec;
}

double* vector_rand(int n)
{
    srand(time(NULL));
    double* v = vector_alloc(n);
    int i;
    for(i=0; i<n; i++)
        v[i] = (double)rand();
    return v;
}

double* matrix_alloc(int m, int n)
{
    double* mat;
    CHK(mat = calloc(n*m,sizeof(double)));
    return mat;
}

double* matrix_rand(int m, int n)
{
    srand(time(NULL));
    double* mat = matrix_alloc(m,n);
    int i, j;
    for(j=0; j<n; j++)
        for(i=0; i<m; i++)
            mat[j*m+i] = rand_double();
    return mat;
}

void tab_nullify(double* tab, int N)
{
    int i;
    for(i=0;i<N;i++)
        tab[i]=0.0;
}


void matrix_AtoLU(int m, int n, double* a, int lda, double* l, int ldl, double* u, int ldu)
{
  int i,j, min = (m<n)?m:n;
  for (i = 0; i < m; i++)
    for (j = 0; j < min; j++)
      l[i+j*ldl] = 0.0;
  for (i = 0; i < min; i++)
    for (j = 0; j < n; j++)
      u[i+j*ldu] = 0.0;
  for (i = 0; i < min; i++)
    for (j = i+1; j < n; j++)
      u[i+j*ldu] = a[i+j*lda];
  for (i = 0; i < min; i++)
    u[i+i*ldu] = 1.0;
  for (i = 0; i < m; i++)
    for (j = 0; (j <= i) && (j < min); j++)
      l[i+j*ldl] = a[i+j*lda];
};

void affiche(int m, int n, double* a, int lda, FILE* flux)
{
    int i, j;
    for(i=0; i<m; i++)
    {
        for(j=0; j<n; j++)
            fprintf(flux, "%.2e ", a[lda*j+i]);
        fprintf(flux, "\n");
    }
}

int nb_col(int com_size, int bsize, int n, int rank)
{
  int nb_col = n%bsize == 0 ? n/bsize : (n/bsize)+1;
  int i, res = 0;
  for (i = 0; i<nb_col; ++i){
    if (nb_local(i, rank, com_size))
      res++;
  }
  return res;
}

int nb_local(int j, int rank, int size)
{
  int i = j%(size*2);
  int nb = j/(size*2);
  if (i == rank)
    return (1 + nb*2);
  else if (i == (size*2 - 1 - rank))
    return (2 + nb*2);
  return 0;
}

int dest(int j, int size)
{
  int res = j%(size*2);
  if (res < size)
    return res;
  return (size*2 - 1 - res);
}

void split_matrix(int m, int n, double* rootbuf, double* sr_buf, int bsize, enum arrangement arr)
{
    MPI_Datatype column_type_max;
    MPI_Datatype column_type_min;
    MPI_Datatype *column_type;
    int size, rank;
    int nb_col = n%bsize == 0 ? n/bsize : (n/bsize)+1;
    int col_size_max = sizeof(double)*m*bsize;
    int bsize_min = n%bsize == 0 ? bsize : n%bsize;
    int col_size_min = n%bsize == 0 ? col_size_max : (n%bsize)*m*sizeof(double);
    int col_el = m*bsize;
    int col_size = col_size_max;
    column_type = &column_type_max;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Type_contiguous(m*bsize, MPI_DOUBLE, &column_type_max);
    MPI_Type_commit(&column_type_max);
    MPI_Type_contiguous(m*bsize_min, MPI_DOUBLE, &column_type_min);
    MPI_Type_commit(&column_type_min);

    int i, local;
    if (arr == SCATTER) {
      for (i = 0; i< nb_col; ++i){
	if ((i+1) == nb_col){
	  col_size = col_size_min;
	  column_type = &column_type_min;
	}
	local = nb_local(i, rank, size);
	if (local && (rank != 0)){
	  MPI_Recv(sr_buf+(local-1)*col_el, 1, *column_type, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else if (local && (rank == 0)){
	  memcpy(sr_buf+(local-1)*col_el, rootbuf+i*col_el, col_size);
	} else if (rank == 0){
	  MPI_Send(rootbuf+i*col_el, 1, *column_type, dest(i, size), 99, MPI_COMM_WORLD);
	}
      }
    } else if (arr == GATHER){
      for (i = 0; i< nb_col; ++i){
	if ((i+1) == nb_col){
	  col_size = col_size_min;
	  column_type = &column_type_min;
	}
	local = nb_local(i, rank, size);
	if (local && (rank != 0)){
	  MPI_Send(sr_buf+(local-1)*col_el, 1, *column_type, 0, 99, MPI_COMM_WORLD);
	} else if (local && (rank == 0)){
	  memcpy(rootbuf+i*col_el, sr_buf+(local-1)*col_el, col_size);
	} else if (rank == 0) {
	  MPI_Recv(rootbuf+i*col_el, 1, *column_type, dest(i, size), 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
      }
    }
}
