#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <assert.h>

#include "fox_mult/mult.h"
#include "util/proc.h"
#include "util/matrix.h"

#define FILENAMES_LEN 100

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define ABS(a) ((a)>0?(a):-(a))
#define RELATIVE_ERROR 0.000001
#define ASSERT_EQ(a,b) if(ABS(((a)-(b))/(a))>RELATIVE_ERROR){printf(#a "!=" #b "(%e!=%e) (diff rel = %e)\n",a,b,ABS(((a)-(b))/(a))); assert(0);}

#define DEFAULT_MAT_SIZE 4

void print_help()
{
  printf("---------Particles---------\n");
  printf("Usage :\n");
  printf(" -a : input file for matrix A, random if unspecified (size = " STR(DEFAULT_MAT_SIZE) ").\n");
  printf(" -b : input file for matrix B, random if unspecified (size = " STR(DEFAULT_MAT_SIZE) ").\n");
  printf(" -o : output file for matrix C - stdout if unspecified.\n");
  printf(" -c : input file for exact result matrix C, check if the relative error is acceptable.\n");
}



int main(int argc, char** argv){

  struct matrix A;
  struct matrix B;
  struct matrix C;
  struct matrix D;

  struct matrix local_A;
  struct matrix local_B;
  struct matrix local_C;

  struct grid grid;
  MPI_Comm comm;
  int nb_proc_row;
  int nb_proc_tot;
  int rank;
  int nb_in_block;

  int err = 0;
  int i, mat_size = DEFAULT_MAT_SIZE;
  char a_filename[FILENAMES_LEN];
  char b_filename[FILENAMES_LEN];
  char d_filename[FILENAMES_LEN];
  char o_filename[FILENAMES_LEN];

  a_filename[0] = '\0';
  b_filename[0] = '\0';
  d_filename[0] = '\0';
  o_filename[0] = '\0';

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc_tot);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (EXIT_FAILURE == compute_communicator(nb_proc_tot,&nb_proc_row,&comm,&rank)){
    MPI_Finalize();
    return EXIT_FAILURE;
  }
  
  mult_fox_mpi_init(nb_proc_row, &comm, &grid, rank);  

  if (rank == 0){

    for(i=1; i<argc; i++){
      if(argv[i][0]!='-'){
	printf("Invalid command : %s\n", argv[i]);
	err++;
	break;
      }
      if(argv[i][1]=='a'){
	int res = snprintf(a_filename,FILENAMES_LEN-1,"%s",argv[++i]);
	if(res>FILENAMES_LEN)
	  printf("WARNING:input filename probably too long\n");
	a_filename[FILENAMES_LEN-1]='\0';
	load_matrix_from_file(&A, nb_proc_row, a_filename);
      } else if(argv[i][1]=='b'){
	int res = snprintf(b_filename,FILENAMES_LEN-1,"%s",argv[++i]);
	if(res>FILENAMES_LEN)
	  printf("WARNING:input filename probably too long\n");
	b_filename[FILENAMES_LEN-1]='\0';
	load_matrix_from_file(&B, nb_proc_row, b_filename);
      } else if(argv[i][1]=='c'){
	int res = snprintf(d_filename,FILENAMES_LEN-1,"%s",argv[++i]);
	if(res>FILENAMES_LEN)
	  printf("WARNING:input filename probably too long\n");
	d_filename[FILENAMES_LEN-1]='\0';
	load_matrix_from_file(&D, nb_proc_row, d_filename);
      } else if(argv[i][1]=='o'){
	int res = snprintf(o_filename,FILENAMES_LEN-1,"%s",argv[++i]);
	if(res>FILENAMES_LEN)
	  printf("WARNING:output filename probably too long\n");
	o_filename[FILENAMES_LEN-1]='\0';
      } else {
	printf("Invalid command : %s\n", argv[i]);
	err++;
	break;
      }
    }

    if (a_filename[0] == '\0'){
      create_random_matrix(&A, DEFAULT_MAT_SIZE, nb_proc_row);
    } else {
      mat_size = A.length;
    }
    if (b_filename[0] == '\0'){
      create_random_matrix(&B, DEFAULT_MAT_SIZE, nb_proc_row);
    }
    create_random_matrix(&C, mat_size, nb_proc_row);
    if ((A.length != B.length)||((d_filename[0] != '\0') && (A.length != D.length))){
      err++;
      printf("Attention, les tailles des matrices dans les fichiers ne correspondent pas, fin du programme.\n");
    }
    nb_in_block = A.length/nb_proc_row;
  }

  MPI_Bcast(&nb_in_block, 1, MPI_INT, 0, comm);
  MPI_Bcast(&err, 1, MPI_INT, 0, comm);

  if (err > 0){
    if (rank == 0){
      free(A.tab);
      free(B.tab);
      free(C.tab);
      if (d_filename[0] != '\0'){
	free(D.tab);
      }
    }
    mult_fox_mpi_finalize(&grid);
    MPI_Comm_free(&comm);
    MPI_Finalize();
    return EXIT_FAILURE;
  }      

  create_random_matrix(&local_A, nb_in_block, 1);
  create_random_matrix(&local_B, nb_in_block, 1);
  create_random_matrix(&local_C, nb_in_block, 1);

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, A.tab, local_A.tab, SCATTER);
  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, B.tab, local_B.tab, SCATTER);  

  mult_fox_mpi(nb_in_block, local_A.tab, local_B.tab, local_C.tab, &grid, comm);

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, local_C.tab, C.tab, GATHER);

  free(local_A.tab);
  free(local_B.tab);
  free(local_C.tab);

  if (rank == 0){
    if (d_filename[0] != '\0'){
      for (i = 0; i<(C.length*C.length); i++){
	ASSERT_EQ(C.tab[i],D.tab[i]);
      }
      printf("Matrix result is okay ... (err < "STR(RELATIVE_ERROR)").\n");
      free(D.tab);
    }
    if (o_filename[0] != '\0'){
      FILE* file = fopen(o_filename, "w");
      if (file != NULL){
	print_matrix(&C, file);
	fclose(file);
      } else {
	printf("Error when opening output file.\n");
      }
    } else {
      print_matrix(&C, stdout);
    }

    free(A.tab);
    free(B.tab);
    free(C.tab);
  }

  mult_fox_mpi_finalize(&grid);
  MPI_Comm_free(&comm);
  MPI_Finalize();

  return EXIT_SUCCESS;
}
