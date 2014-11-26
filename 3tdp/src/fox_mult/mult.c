#include <mpi.h>
#include <stdlib.h>

#include "mult.h"
#include "util/cblas_mkl.h"
#include "util/matrix.h"

#define SWAP(a,b) void* c = (b); (b) = (a); (a) = c

#define COMM_TAG_BKI 42

int mult_fox_mpi_init(int nb_proc_row, MPI_Comm* comm, struct grid* grid, int rank){
  const int remain_dims[] = {0, 1};
  MPI_Cart_sub(*comm, remain_dims, &grid->comm_row);
  grid->N = nb_proc_row;
  int coords[2];
  MPI_Cart_coords(*comm, rank, 2, coords);
  grid->rank_I = coords[0];
  grid->rank_J = coords[1];
  int r;
  coords[0]++; 
  MPI_Cart_rank(*comm, coords, &r);
  grid->proc_under = r;
  coords[0] -= 2; 
  MPI_Cart_rank(*comm, coords, &r);
  grid->proc_above = r;
  return EXIT_SUCCESS;
}


int mult_fox_mpi_finalize(struct grid* grid){
  MPI_Comm_free(&grid->comm_row);
  return EXIT_SUCCESS;
}


void mult_fox_mpi(int n, const double* Aij, const double* Bij, double* Cij, const struct grid* grid, MPI_Comm comm){
    int I = grid->rank_I;
    int J = grid->rank_J;
    int N = grid->N;
    int i,c;
    
    double* Aik = malloc(sizeof(double)*n*n);
    double* Bkj = malloc(sizeof(double)*n*n);
    double* Bkj_next = malloc(sizeof(double)*n*n);

    for(i = 0; i<n*n; i++) Cij[i] = 0.0;

    for(c=0; c<N; c++){
        int k = (I+c)%N;
        //Send direcltly Bij at 1st send (avoid copy to Bkj)
        const double* Bkj_current = (k==I)?Bij:Bkj;
        MPI_Request bkj_send_request, bkj_recv_request;

        MPI_Isend(Bkj_current, n*n, MPI_DOUBLE, grid->proc_above, COMM_TAG_BKI, comm, &bkj_send_request);
        MPI_Irecv(Bkj_next,    n*n, MPI_DOUBLE, grid->proc_under, COMM_TAG_BKI, comm, &bkj_recv_request);

	//Broadcast direcltly Aij when k==j (avoid copy to Akj)
	const double* Aik_current = (k==J)?Aij:Aik;
	MPI_Bcast((double*)Aik_current,n*n,MPI_DOUBLE,k,grid->comm_row);
	
	//Cij += Aik*Bkj
	cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,n,n,n,1.0,Aik_current,n,Bkj_current,n,1.0,Cij,n);
   
        MPI_Request requests[] = {bkj_send_request, bkj_recv_request};
        MPI_Waitall(2,requests,MPI_STATUSES_IGNORE);
        SWAP(Bkj,Bkj_next);
    }
    
    free(Aik);
    free(Bkj);
    free(Bkj_next);
}
