#ifndef __UTIL_MPI__H
#define __UTIL_MPI__H

#include <mpi.h>

/**
 * Represents the structure of the computation grid
 **/
struct grid{
    int rank_I;         ///Line rank
    int rank_J;         ///Column rank
    int proc_above;     ///Rank of the process above (i+1,j) in the grid
    int proc_under;     ///Rank of the process under (i-1,j) in the grid
    int proc_left;      ///Rank of the process at left (i,j-1) in the grid
    int proc_right;     ///Rank of the process at right (i,j+1) in the grid
};


enum arrangement {GATHER, SCATTER};

/*
 * Compute a new communicator, with the initial number of processors.
 * \param nb_proc_row is set (but not allocated) by the function, to 
 * the number of processors used per row (or column).
 * \param new_comm is set (but not allocated) by the function, to
 * the communicator with only the processors to be used by the computation.
 */
int compute_communicator(int nb_proc_tot, int* nb_proc_row, MPI_Comm* new_comm, int* rank);


/*
 * Scatter (or gather) the matrix to (from) all the process.
 * nb_proc_row and nb_in_block are used to calculate sizes of blocks to transmit
 * comm is the grid communicator
 * type is GATHER or SCATTER
 */
int matrix_placement_proc(int nb_proc_row, int nb_in_block, MPI_Comm* comm, double* sendbuf, double* rcvbuf, enum arrangement type);

/*
 * Fill the grid fields with some appropriate values
 *
 */
int mpi_grid_init(MPI_Comm* comm, struct grid* grid, int rank);


#endif
