#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include <mpi.h>

#include "util_mpi.h"

//#define PRINT_ALIVE
static int BS = 1000;

#define global_cell( _i_, _j_ ) global_board[ ldboard * ((_j_) - 1) + ((_i_) - 1) ]
#define cell( _i_, _j_ ) board[ ldlboard * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

double mytimer(void)
{
  struct timeval tp;
  gettimeofday( &tp, NULL );
  return tp.tv_sec + 1e-6 * tp.tv_usec;
}

void output_board(int N, int *global_board, int ldboard, int loop)
{
  int i,j;
  printf("loop %d\n", loop);
  for (i=1; i<=N; i++) {
    for (j=1; j<=N; j++) {
      if ( global_cell( i, j ) == 1)
	printf("X");
      else
	printf(" ");
    }
    printf("\n");
  }
}

void output_lboard(int N, int *board, int ldlboard, int loop)
{
  int i,j;
  printf("loop %d\n", loop);
  for (i=1; i<=N; i++) {
    for (j=1; j<=N; j++) {
      if ( cell( i, j ) == 1)
	printf("X");
      else
	printf(" ");
    }
    printf("\n");
  }
}


/**
 * This function generates the initial board with one row and one
 * column of living cells in the middle of the board
 */
int generate_initial_board(int *global_board, int ldboard)
{
  int i, j, num_alive = 0;

  for (i = 1; i <= BS; i++) {
    for (j = 1; j <= BS; j++) {
      if (i == BS/2 || j == BS/2) {
	global_cell(i, j) = 1;
	num_alive ++;
      }
      else {
	global_cell(i, j) = 0;
      }
    }
  }

  return num_alive;
}

int main(int argc, char* argv[])
{
  int i, j, loop, num_alive, maxloop;
  int ldboard, ldnbngb, ldlboard;
  double t1, t2;
  double temps;
 
  int *board;
  int *nbngb;

  int local_alive;
  int *global_board;

  struct grid grid;
  MPI_Comm comm;
  int nb_proc_row;
  int nb_proc_tot;
  int rank;
  int nb_in_block;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc_tot);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // initialization of the grid communicator
  if (EXIT_FAILURE == compute_communicator(nb_proc_tot,&nb_proc_row,&comm,&rank)){
    MPI_Finalize();
    return EXIT_SUCCESS;
  }


  if (argc < 2) {
    maxloop = 10;
  } else if (argc > 2){
    maxloop = atoi(argv[1]);
    BS = atoi(argv[2]);
  } else
    maxloop = atoi(argv[1]);
  num_alive = 0;
  local_alive = 0;

  /* Leading dimension of the board array */
  ldboard = BS;
  if (ldboard % nb_proc_row != 0){
    if (rank == 0)
      printf("Wrong BS (or wrong number of procs) ... exiting now.\n");
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  /* Leading dimension of the neigbour counters array */
  nb_in_block = ldboard / nb_proc_row;
  ldnbngb = nb_in_block;
  ldlboard = nb_in_block + 2;

  board = malloc( ldlboard * ldlboard * sizeof(int) );
  nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

  if (rank == 0){
    global_board = malloc( ldboard * ldboard * sizeof(int) );
    num_alive = generate_initial_board( &(global_cell(1, 1)), ldboard );
    printf("Starting number of living cells = %d\n", num_alive);
    t1 = mytimer();
  }

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, &(global_cell( 1, 1)), &(cell( 1, 1)), SCATTER, ldlboard);

  mpi_grid_init(&comm, &grid, rank);

  //output_lboard( nb_in_block, &(cell(1, 1)), ldlboard, 0 );

  /* for (loop = 1; loop <= maxloop; loop++) { */

  /*   cell(   0, 0   ) = cell(BS, BS); */
  /*   cell(   0, BS+1) = cell(BS,  1); */
  /*   cell(BS+1, 0   ) = cell( 1, BS); */
  /*   cell(BS+1, BS+1) = cell( 1,  1); */

  /*   for (i = 1; i <= BS; i++) { */
  /*     cell(   i,    0) = cell( i, BS); */
  /*     cell(   i, BS+1) = cell( i,  1); */
  /*     cell(   0,    i) = cell(BS,  i); */
  /*     cell(BS+1,    i) = cell( 1,  i); */
  /*   } */

  /*   //calcul du nombre de voisins */
  /*   for (j = 1; j <= BS; j++) { */
  /*     for (i = 1; i <= BS; i++) { */
  /* 	ngb( i, j ) = */
  /* 	  cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) + */
  /* 	  cell( i-1, j   ) +                  cell( i+1, j   ) + */
  /* 	  cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 ); */
  /*     } */
  /*   } */

  /*   //mise à jour de la matrice */
  /*   num_alive = 0; */
  /*   for (j = 1; j <= BS; j++) { */
  /*     for (i = 1; i <= BS; i++) { */
  /* 	if ( (ngb( i, j ) < 2) ||  */
  /* 	     (ngb( i, j ) > 3) ) { */
  /* 	  cell(i, j) = 0; */
  /* 	} */
  /* 	else { */
  /* 	  if ((ngb( i, j )) == 3) */
  /* 	    cell(i, j) = 1; */
  /* 	} */
  /* 	if (cell(i, j) == 1) { */
  /* 	  num_alive ++; */
  /* 	} */
  /*     } */
  /*   } */

  /*     //output_lboard( nb_in_block, &(cell(1, 1)), ldlboard, loop); */
  /* #ifdef PRINT_ALIVE */
  /*     if (rank == 0)*/
  /*       printf("%d \n", num_alive); */
  /* #endif */
  /*   } */

  matrix_placement_proc(nb_proc_row, nb_in_block, &comm, &(cell( 1, 1)), &(global_cell( 1, 1)), GATHER, ldlboard);
  MPI_Reduce(&local_alive, &num_alive, 1, MPI_INT, MPI_SUM, 0, comm);

  if (rank == 0){
    t2 = mytimer();
    temps = t2 - t1;
    printf("Final number of living cells = %d\n", num_alive);
    printf("time=%.2lf ms\n",(double)temps * 1.e3);
    
    //output_board( BS, &(global_cell(1, 1)), ldboard, maxloop);
    free(global_board);
  }
  free(board);
  free(nbngb);

  MPI_Comm_free(&comm);
  MPI_Finalize();

  return EXIT_SUCCESS;
}

