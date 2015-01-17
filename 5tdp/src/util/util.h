#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <time.h>

enum arrangement {GATHER, SCATTER};

// Allocation de vecteurs et matrices, avec initialisation aléatoire 
// des valeurs si _rand
double* vector_alloc(int n);
double* vector_rand(int n);
double* matrix_alloc(int m, int n);
double* matrix_rand(int m, int n);
// Remise à zéro des N éléments d'un tableau
void tab_nullify(double* tab, int N);
// Remplissage de L et U à partir de la représentation condensée dans A
void matrix_AtoLU(int m, int n, double* a, int lda, double* l, int ldl, double* u, int ldu);
// Affichage d'une matrice m X n dans le flux spécifié
void affiche(int m, int n, double* a, int lda, FILE* flux);
// Combien de colonne ?
int nb_col(int com_size, int bsize, int n, int rank);
// Est-ce qu'un indice j de bloc est local ou non (colonne)
// si oui, numéro de colonne interne (à partir de 1) si non 0
int nb_local(int j, int rank, int size);
// Éclatement en colonne d'une matrice sur tous les processus
void split_matrix(int m, int n, double* rootbuf, double* sr_buf, int bsize, enum arrangement arr);

#endif
