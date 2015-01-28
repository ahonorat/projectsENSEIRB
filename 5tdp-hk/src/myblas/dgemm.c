#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "myblas.h"

//fonctionne uniquement pour col major
int coord_cm(const int i, const int j, const MYBLAS_TRANSPOSE Trans, const int ld){
  if (Trans == CblasTrans){
    return i*ld+j;
  } else {
    return j*ld+i;
  }
}



void myblas_dgemm_jik(const MYBLAS_ORDER Order, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc)
{
    assert(Order == CblasColMajor);
    assert(beta == 1.0);

    int i, j, k;
    for(j = 0; j<N; j++)
        for(i = 0; i<M; i++)
        {
            double c = 0;
            for(k = 0; k<K; k++)
	      c += A[coord_cm(i,k,TransA,lda)]*B[coord_cm(k,j,TransB,ldb)];
            C[coord_cm(i,j,CblasNoTrans,ldc)] += alpha*c;
        }
}


#define A(i,j) &A[((i)+lda*(j))]
#define B(i,j) &B[((i)+ldb*(j))]
#define C(i,j) &C[((i)+ldc*(j))]
#define MIN(a,b) ((a)<(b))?(a):(b)

/** dgemm_bloc avec la taille de blocs en paramètres */
void myblas_dgemm_bloc_aux(const MYBLAS_TRANSPOSE TransA, const MYBLAS_TRANSPOSE TransB, 
			   int bloc_size_m, int bloc_size_n, int bloc_size_k,
			   const int M, const int N,
			   const int K, const double alpha, const double *A,
			   const int lda, const double *B, const int ldb,
			   const double beta, double *C, const int ldc)
{
    int m,n,k;
    for(m=0;m<M;m+=bloc_size_m)
    {
        int m_bloc_size = MIN(bloc_size_m,M-m);
        for(n=0;n<N;n+=bloc_size_n)
        {
            int n_bloc_size = MIN(bloc_size_n,N-n);
            for(k=0;k<K;k+=bloc_size_k)
	      myblas_dgemm_jik(CblasColMajor,TransA,TransB,m_bloc_size,n_bloc_size,MIN(bloc_size_k,K-k),
			       alpha,A(k,m),lda,B(k,n),ldb,beta,C(m,n),ldc);
        }
    }
}

void myblas_dgemm_bloc(const MYBLAS_ORDER Order, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc)
{
    assert(Order == CblasColMajor);
    assert(beta == 1.0);

    myblas_dgemm_bloc_aux(TransA,TransB,512,512,1024,M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
}


// Structure permettant de passer les informations nécessaires aux threads
// pour le calcul par bloc parallèle. Contient tous les arguments du dgemm.
struct bloc_info{
    const double *A;
    const double *B;
    double *C;
    int m_bloc_size, n_bloc_size, k_bloc_size;
    int m, n, k;
    double alpha, beta;
    int lda, ldb, ldc;
};


// Fonction des threads
void* bloc_thread(void* data){
    struct bloc_info* bloc = (struct bloc_info*)data;

    const double *A = bloc->A;
    const double *B = bloc->B;
    double *C = bloc->C;
    int lda = bloc->lda;
    int ldb = bloc->ldb;
    int ldc = bloc->ldc;

    myblas_dgemm_bloc(CblasColMajor,     CblasTrans,         CblasNoTrans,
                    bloc->m_bloc_size,  bloc->n_bloc_size,  bloc->k_bloc_size,
                    bloc->alpha,
                    A(0,bloc->m),       bloc->lda,
                    B(0,bloc->n),       bloc->ldb,
                    bloc->beta,
                    C(bloc->m,bloc->n), bloc->ldc);
    return NULL;
}


void myblas_dgemm_bloc_parallel(const MYBLAS_ORDER Order, const MYBLAS_TRANSPOSE TransA,
                 const MYBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc)
{
  //Une première étape détermine le nombre de bloc de C traités en
  //parallèle, qui est égal au nombre de processeurs.
  //Le résultat est static, car le résultat est le même pour tous les bench,
  //nous ne voulons pas être influencé par un changement du nombre de threads
  //pendant l'exécution.
    static int myblas_dgemm_nbProc = -1;
    static int b_m, b_n;
    if(myblas_dgemm_nbProc == -1){
        char* s = getenv("MYLIB_NUM_THREADS");
        if(s!=NULL)
            myblas_dgemm_nbProc = atoi(s);
        else
            myblas_dgemm_nbProc = 2;
        int i;
        for(i=1;i*i<=myblas_dgemm_nbProc; i++){
            if(myblas_dgemm_nbProc%i == 0){
                b_m = i;
                b_n = myblas_dgemm_nbProc/i;
            }
        }
    }
	
    //Calcul de la taille des blocs pour en avoir autant que de processeurs
    int size_bloc_m=M/b_m+1;
    int size_bloc_n=N/b_n+1;

    assert(Order == CblasColMajor);
    assert(TransA == CblasTrans);
    assert(TransB == CblasNoTrans);
    assert(beta == 1.0);

    int m,n;
    int i=0,j;
    pthread_t thread[myblas_dgemm_nbProc];
    struct bloc_info bloc[myblas_dgemm_nbProc];

    for(m=0;m<M;m+=size_bloc_m)
    {
        int m_bloc_size = MIN(size_bloc_m,M-m);
        for(n=0;n<N;n+=size_bloc_n)
        {
            int n_bloc_size = MIN(size_bloc_n,N-n);
            struct bloc_info current_bloc = {A,B,C,m_bloc_size,n_bloc_size,K,m,n,0,alpha,beta,lda,ldb,ldc};
            bloc[i] = current_bloc;
	    //création d'un thread par bloc
            pthread_create(&thread[i], NULL, bloc_thread, &bloc[i]);
            i++;
        }
    }
    for(j=0;j<i;j++)
      pthread_join(thread[j],NULL);
}
