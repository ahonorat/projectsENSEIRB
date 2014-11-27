#ifndef DGEMM_DIST_H
#define DGEMM_DIST_H

#include <mpi.h>

#include "mult.h"

/**
 * Represents the structure of the computation grid
 **/
struct grid{
    MPI_Comm comm_row;  ///Communicator to broadcast to the current row
    int rank_I;         ///Line rank
    int rank_J;         ///Column rank
    int N;              ///Number of blocks in a line
    int proc_above;     ///Rank of the process above (i+1,j) in the grid
    int proc_under;     ///Rank of the process under (i-1,j) in the grid
};

/**
 * Create the computation grid needed by mult_fox_mpi
 * nb_proc_row is the number of proc by row, and by column
 * comm is the carthesian communicator
 * grid is the grid to initialize
 * rank us the rank of calling process
 **/
int mult_fox_mpi_init(int nb_proc_row, MPI_Comm* comm, struct grid* grid, int rank);

/**
 * C = A*B with the fox multiplication algorithm
 * Aij, Bij, Cij are the distributed blocs of the matrix
 * n is the size of the blocks
 * comm is the grid communicator
 * grid represents the topology of the computation grid
 *      (i.e. how the other blocs are distributed)
 **/
void mult_fox_mpi(int n, const double* Aij, const double* Bij, double* Cij, const struct grid* grid, MPI_Comm comm);

/**
 * Frees the ressources for the grid (communicators, topology data ...)
 * The distributed matrices are not freed in the process
 **/
int mult_fox_mpi_finalize(struct grid* grid);

#endif
