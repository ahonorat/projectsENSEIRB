#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>

// Allocation de vecteurs et matrices, avec initialisation aléatoire 
// des valeurs si _rand
double* vector_alloc(int n);
double* vector_rand(int n);
double* matrix_alloc(int m, int n);
double* matrix_rand(int m, int n);
// Affichage d'une matrice m X n dans le flux spécifié
void affiche(int m, int n, double* a, int lda, FILE* flux);

#endif
