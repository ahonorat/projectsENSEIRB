#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"

/*
 * Vérification visuelle de la fonction d'affichage.
 * Affichage d'une matrice complète, puis d'un sous-bloc.
 */
int main(){
    double* mat = matrix_rand(4,6);
    affiche(4, 6, mat, 4, stdout);
    fprintf(stdout, "Sous-bloc : \n");
    affiche(2, 3, mat+2+4, 4, stdout);
    return 0;
}
