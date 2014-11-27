#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include "fox_mult/mult.h"

/**
 * Structure describing a matrix
 * The matrix is row major
 */
struct matrix{
  double* tab;  /// Pointer to the Top-Left corner of the matrix
  int length;   /// Size of a line
  //Tab must be a length*length array of double
};

int randomize_matrix(struct matrix* mat, int nb_row);
int load_matrix_from_file(struct matrix* mat, int nb_proc_row, const char* filename);
int create_random_matrix(struct matrix* mat, int nb_row, int nb_proc_row);
int print_matrix(struct matrix* mat, FILE* file);


#endif
