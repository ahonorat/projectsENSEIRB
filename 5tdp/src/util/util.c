#include <stdio.h>
#include <stdlib.h>
#include <float.h>

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
