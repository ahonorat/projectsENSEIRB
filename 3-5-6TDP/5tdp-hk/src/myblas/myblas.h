/* Contient les prototypes de nos routines BLAS
 */

#ifndef MYBLAS_H
#define MYBLAS_H

// Définition des constantes, suivant le BLAS utilisé (mkl ou non).
#ifdef USE_MKL
#include <mkl.h>
#define MYBLAS_TRANSPOSE CBLAS_TRANSPOSE
#define MYBLAS_ORDER CBLAS_LAYOUT //CBLAS_ORDER works too
#define MYBLAS_SIDE CBLAS_SIDE
#define MYBLAS_UPLO CBLAS_UPLO
#define MYBLAS_DIAG CBLAS_DIAG
#else
#include <cblas.h>
#define MYBLAS_TRANSPOSE enum CBLAS_TRANSPOSE //CblasNoTrans || CblasTrans (|| CblasConjTrans)
#define MYBLAS_ORDER enum CBLAS_ORDER //CblasColMajor (|| CblasRowMajor)
#define MYBLAS_SIDE enum CBLAS_SIDE //CblasLeft || CblasRight
#define MYBLAS_UPLO enum CBLAS_UPLO //CblasUpper || CblasLower
#define MYBLAS_DIAG enum CBLAS_DIAG //CblasNonUnit || CblasUnit
#endif

// Fonctions ddot (la seconde différencie le cas où incX==incY==1)
double myblas_ddot_seq(const int N, const double *X, const int incX, const double *Y, const int incY);
double myblas_ddot_seq_inc(const int N, const double *X, const int incX, const double *Y, const int incY);


// Fonctions dgemm
void myblas_dgemm_jik(const MYBLAS_ORDER Order, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc);
void myblas_dgemm_bloc(const MYBLAS_ORDER Order, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc);
void myblas_dgemm_bloc_parallel(const MYBLAS_ORDER Order, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc);

/*
 * dgemm_bloc avec la taille de blocs en paramètres
 * Ne doit pas être appelé par l'utilisateur car aucune vérification
 * n'est faite sur MY_BLAS_ORDER ou MY_BLANS_TRANSPOSE des matrices A et B,
 * ni sur la validité des tailles de bloc fournies.
 */
void myblas_dgemm_bloc_aux(const MYBLAS_TRANSPOSE TransA, const MYBLAS_TRANSPOSE TransB, 
			   int bloc_size_m, int bloc_size_n, int bloc_size_k,
			   const int M, const int N,
			   const int K, const double alpha, const double *A,
			   const int lda, const double *B, const int ldb,
			   const double beta, double *C, const int ldc);

// Autres routines de la librairie --> TDP5
void myblas_daxpy(int N, double alpha, const double *X, int incX, double *Y, int incY);
void myblas_dger(const MYBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda);
void myblas_dscal(const int N, const double alpha, double *X, const int incX);
void myblas_dtrsm(const MYBLAS_ORDER Order, const MYBLAS_SIDE Side,
                 const MYBLAS_UPLO Uplo, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_DIAG Diag, const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 double *B, const int ldb);


#endif
