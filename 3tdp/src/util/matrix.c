#include <stdlib.h>
#include <time.h>

#include "matrix.h"

#define RANDOM_FLOAT(max) ((float)(rand())/(float)(RAND_MAX)*2-1.0)*(max)
#define MAX_FLOAT 10.0

int load_matrix_from_file(struct matrix* mat, int nb_proc_row, const char* filename){
  FILE* file = fopen(filename, "r");
  if(file == NULL){
    perror("Cannot open matrix file");
    return EXIT_FAILURE;
  }

  int count;
  if (1 != fscanf(file,"%d", &count) || count<0){
    fclose(file);
    return EXIT_FAILURE;
  }

  mat->length = (count/nb_proc_row);
  mat->length *= nb_proc_row;
  if (count%nb_proc_row != 0){
    mat->length +=nb_proc_row;
  }

  mat->tab = malloc(sizeof(double)*mat->length*mat->length);
  int k,l;
  for (k=count; k<mat->length; k++){
    for (l=0; l<mat->length; l++){
      mat->tab[k*mat->length+l] = 0.0;
    }
  }
  for (k=0; k<count; k++){
    for (l=count; l<mat->length; l++){
      mat->tab[k*mat->length+l] = 0.0;
    }
  }


  int i,j;
  for(i=0; i<count; i++){
    j = 0;
    if(1 != fscanf(file, "\n%lf", &mat->tab[i*mat->length+j])){
      fclose(file);
      return EXIT_FAILURE;
    }
    for(j=1; j<count; j++){
      if(1 != fscanf(file, "%lf", &mat->tab[i*mat->length+j])){
	fclose(file);
	return EXIT_FAILURE;
      }
    }
  }

  double f;
  if(EOF != fscanf(file,"%lf", &f))
    printf("WARNING: file longer than expected\n");
  fclose(file);
  return EXIT_SUCCESS;
}


int create_random_matrix(struct matrix* mat, int nb_row, int nb_proc_row){
  srand(time(NULL));
  int count = nb_row;
  mat->length = (count/nb_proc_row);
  mat->length *= nb_proc_row;
  if (count%nb_proc_row != 0){
    mat->length +=nb_proc_row;
  }

  mat->tab = malloc(sizeof(double)*mat->length*mat->length);
  int k,l;
  for (k=count; k<mat->length; k++){
    for (l=0; l<mat->length; l++){
      mat->tab[k*mat->length+l] = 0.0;
    }
  }
  for (k=0; k<count; k++){
    for (l=count; l<mat->length; l++){
      mat->tab[k*mat->length+l] = 0.0;
    }
  }

  int i,j;
  for(i=0; i<count; i++){
    for(j=0; j<count; j++){
     mat->tab[i*mat->length+j] = RANDOM_FLOAT(MAX_FLOAT);
    }
  }
  return EXIT_SUCCESS;
}


int print_matrix(struct matrix* mat, FILE* file){
  int i;
  for(i = 0; i<(mat->length*mat->length); i++){
    fprintf(file, "%lf ", mat->tab[i]);
    if (((i+1) % mat->length) == 0)
      fprintf(file, "\n");
  }
  return EXIT_SUCCESS;
}
