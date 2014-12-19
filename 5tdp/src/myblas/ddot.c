#include "myblas.h"

double myblas_ddot_seq(const int N, const double *X, const int incX, const double *Y, const int incY)
{
    double ddot = 0;
    int i;
    for(i=0; i<N; i++)
    {
        ddot += X[i*incX]*Y[i*incY];
    }
    return ddot;
}

double myblas_ddot_seq_inc(const int N, const double *X, const int incX, const double *Y, const int incY)
{
    double ddot = 0;
    if(incX==1 && incY==1)
    {
        int i;
        for(i=0; i<N; i++)
            ddot += X[i]*Y[i];//Plus de multiplication entière
    }
    else
    {
        int i;
        for(i=0; i<N; i++)
            ddot += X[i*incX]*Y[i*incY];
    }
    return ddot;
}
