#ifndef __CONCAT_H__
#define __CONCAT_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//concatène la str2 à str1, la mémoire de str1 est réallouée, celle de str2 détruite
char *concat(char *str1,char *str2);
//nouveau label, déjà alloué
char *label();
//nouveau reg, déjà alloué
char *registre();
//nouveau cond, déjà alloué
char* condition();
//nouveau pointeur déjà alloué
char* pointeur();
//déclarations variables
char *entete_func();
//rajoute les bons paramètres aux fonctions
char *params_func(char *func);

#endif
