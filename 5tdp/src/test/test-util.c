#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <mpi.h>

#include "util/util.h"

#define ABS(a) ((a)>0?(a):-(a))
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>0.000001){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

/*
 * Vérification visuelle de la fonction d'affichage.
 * Affichage d'une matrice complète, puis d'un sous-bloc.
 * Vérification du scatter/gather
 */
int main(){

  double* global_mat;
  double* global_verif;

  MPI_Init(NULL, NULL);
  int size, rank, i;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if (rank == 0){
    double* mat = matrix_rand(4,6);
    printf("Matrice entière de taille (4,6) : \n");
    affiche(4, 6, mat, 4, stdout);
    printf("Sous-bloc [3,2] de taille (2,3) : \n");
    affiche(2, 3, mat+2+4, 4, stdout);
    free(mat);
    printf("Test scatter/gather...\t");
    global_verif = matrix_rand(100, 100);
    global_mat = matrix_rand(100, 100);
    memcpy(global_verif, global_mat, 100*100*sizeof(double));
  }
  int bsize = 3;
  int nb_cols = nb_col(size, bsize, 100, rank);
  double* local_mat = matrix_rand(100, nb_cols*bsize);
  split_matrix(100, 100, global_mat, local_mat, bsize, SCATTER);
  split_matrix(100, 100, global_mat, local_mat, bsize, GATHER);
  if (rank == 0){
    for(i=0;i<100*100;++i)
      ASSERT_EQ(global_mat[i],global_verif[i]);
    printf("ok\n");
    free(global_verif);
    free(global_mat);
  }
  free(local_mat);

  MPI_Finalize();
  return 0;
}
