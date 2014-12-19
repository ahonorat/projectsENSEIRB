#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#define CHK(X) if((X)<=0) {perror(#X); exit(EXIT_FAILURE);}

double* vector_alloc(int n)
{
    double* vec;
    CHK(vec = malloc(sizeof(double)*n));
    return vec;
}

double* vector_rand(int n)
{
    double* v = vector_alloc(n);
    int i;
    for(i=0; i<n; i++)
        v[i] = (double)rand();
    return v;
}

double* matrix_alloc(int m, int n)
{
    double* mat;
    CHK(mat = malloc(sizeof(double)*n*m));
    return mat;
}

double* matrix_rand(int m, int n)
{
    double* mat = matrix_alloc(m,n);
    int i, j;
    for(j=0; j<n; j++)
        for(i=0; i<m; i++)
            mat[j*m+i] = (double)rand();
    return mat;
}

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
