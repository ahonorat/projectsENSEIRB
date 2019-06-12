#ifndef PROC_H
#define PROC_H

#include <mpi.h>

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

#endif
