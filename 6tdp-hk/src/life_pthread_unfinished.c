#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

//#define PRINT_ALIVE
static int BS = 1000;

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

//Global arrays of semaphore. Each array is of size the number of blocks. There are two of arrays : one for the synchronization (with adjacents blocks) before computing the neighbours, and the other for the synchronization (with adjacent blocks) before updating the cells.
sem_t *sem_before_ngb, *sem_before_update;

//Same coment as above.
pthread_cond_t *cond_before_ngb, *cond_before_update;

//Used in combination with the conds
pthread_mutex_t *mutex_before_ngb, *mutex_before_update;

//Some important variables are declared global, so that we don't have to pass them as arguments to the working threads, and so that they can be accessed easily. Those variable are read-only for the working threads, so we don't have to worry about any concurrent access problem.
int nb_block;
int nb_iter;
int nb_thread;
int block_size;

//Corresponds to the number of block per row/column
int nb_block_1D;

typedef struct _args_t{
  int block;
  int *board;
  int ldboard;
  int *nbngb;
  int ldnbngb;
} args_t;

double mytimer(void)
{
  struct timeval tp;
  gettimeofday( &tp, NULL );
  return tp.tv_sec + 1e-6 * tp.tv_usec;
}

void output_board(int N, int *board, int ldboard, int loop)
{
  int i,j;
  printf("loop %d\n", loop);
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      if ( cell( i, j ) == 1)
	printf("X");
      else
	printf(" ");
    }
    printf("\n");
  }
}

/**
 * This function generates the iniatl board with one row and one
 * column of living cells in the middle of the board
 */
int generate_initial_board(int *board, int ldboard)
{
  int i, j, num_alive = 0;

  for (i = 1; i <= BS; i++) {
    for (j = 1; j <= BS; j++) {
      if (i == BS/2 || j == BS/2) {
	cell(i, j) = 1;
	num_alive ++;
      }
      else {
	cell(i, j) = 0;
      }
    }
  }

  return num_alive;
}

//Allocation and initialization of the semaphores and conds
void setup(int nb_block){
  sem_before_ngb = malloc(nb_block*sizeof(sem_t));
  sem_before_update = malloc(nb_block*sizeof(sem_t));
  cond_before_ngb = malloc(nb_block*sizeof(pthread_cond_t));
  cond_before_update = malloc(nb_block*sizeof(pthread_cond_t));
  mutex_before_ngb = malloc(nb_block*sizeof(pthread_mutex_t));
  mutex_before_update = malloc(nb_block*sizeof(pthread_mutex_t));
  for(int block=0; block<nb_block; ++block){
    //Initial value : 8, because otherwise the algorithm can't start
    sem_init(&sem_before_ngb[block], 0, 8);
    sem_init(&sem_before_update[block], 0, 0);
    pthread_cond_init(&cond_before_ngb[block], NULL);
    pthread_cond_init(&cond_before_update[block], NULL);
    pthread_mutex_init(&mutex_before_ngb[block], NULL);
    pthread_mutex_init(&mutex_before_update[block], NULL);
  }
}

//Computes the neighbours for 1 block
void ngb_1_block(int block, int const * const board, int ldboard, int * const nbngb, int ldnbngb){
  int i_block,j_block;
  i_block = block%nb_block_1D;
  j_block = block/nb_block_1D;

  int i,j,starting_i,starting_j,max_i,max_j;
  starting_j = j_block*block_size + 1;
  starting_i = i_block*block_size + 1;
  max_j = starting_j + block_size;
  max_i = starting_i + block_size;

  for(j=starting_j; j<max_j; ++j){
    for(i=starting_i; i<max_i; ++i){
      ngb( i, j ) =
	cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
	cell( i-1, j   ) +                  cell( i+1, j   ) +
	cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
    }
  }
}

//Updates the cell of one block
void update_1_block(int block, int * const board, int ldboard, int const * const nbngb, int ldnbngb, int* alive){
  int i_block,j_block;
  i_block = block%nb_block_1D;
  j_block = block/nb_block_1D;

  //starting_i and starting_j correspond to the coordinates of the first cell of the block (upper left cell)
  int i,j,starting_i,starting_j,max_i,max_j;
  starting_j = j_block*block_size + 1;
  starting_i = i_block*block_size + 1;
  max_j = starting_j + block_size;
  max_i = starting_i + block_size;

  for(j=starting_j; j<max_j; ++j){
    for(i=starting_i; i<max_i; ++i){
      if ( (ngb( i, j ) < 2) || 
	   (ngb( i, j ) > 3) ) {
	cell(i, j) = 0;
      }
      else {
	if ((ngb( i, j )) == 3)
	  cell(i, j) = 1;
      }
      if (cell(i, j) == 1){
	(*alive)++;
      }
    }
  }

  //Ghost cells (border blocks take care of updating them)

  int k,i_ghost,j_ghost;

  //Upper border
  if(i_block == 0){
    i = 1;
    j = starting_j;
    i_ghost = BS+1;
    j_ghost = starting_j;

    for(k=1; k<=block_size; ++k){
      cell(i_ghost, j_ghost) = cell(i, j);
      ++j;
      ++j_ghost;
    }
    
    //Upper left corner
    if(j_block == 0){
      cell(BS+1, BS+1) = cell(1,1);
    }

    //Upper right corner
    if(j_block == nb_block_1D-1){
      cell(BS+1, 0) = cell(1,BS);
    }
  }

  //Lower border
  if(i_block == nb_block_1D-1){
    i = BS;
    j = starting_j;
    i_ghost = 0;
    j_ghost = starting_j;

    for(k=1; k<=block_size; ++k){
      cell(i_ghost, j_ghost) = cell(i, j);
      ++j;
      ++j_ghost;
    }
    
    //Lower left corner
    if(j_block == 0){
      cell(0, BS+1) = cell(BS,1);
    }

    //Lower right corner
    if(j_block == nb_block_1D-1){
      cell(0, 0) = cell(BS,BS);
    }
  }

  //Left border
  if(j_block == 0){
    i = starting_i;
    j = 1;
    i_ghost = starting_i;
    j_ghost = BS+1;

    for(k=1; k<=block_size; ++k){
      cell(i_ghost, j_ghost) = cell(i,j);
      ++i;
      ++i_ghost;
    }
  }

  //Right border
  if(j_block == nb_block_1D - 1){
    i = starting_i;
    j = BS;
    i_ghost = starting_i;
    j_ghost = 0;

    for(k=1; k<=block_size; ++k){
      cell(i_ghost, j_ghost) = cell(i,j);
      ++i;
      ++i_ghost;
    }
  }
}

//Code executed by one thread
void* life_1_thread(void* args){
  args_t *a = (args_t*)args;
  int block;
  block = a->block;
  for(int iter=1; iter<=nb_iter; ++iter){

    //Synchro
    while(1){
      pthread_cond_wait(&cond_before_ngb[block], &mutex_before_ngb[block]);
      int sem;
      sem_getvalue(&sem_before_ngb[block], &sem);
      //The thread needs to check the value of the semaphore in case he has been woken up by mistake (pthread_cond_wait isn't secure)
      if(sem==8){
	//Reset
	sem_destroy(&sem_before_ngb[block]);
	sem_init(&sem_before_ngb[block], 0, 0);
	break;
      }
    }

    //Count neighbours
    ngb_1_block(a->block, a->board, a->ldboard, a->nbngb, a->ldnbngb);

    //Post to other
    int a1,a2;
    for(a1=-1; a1<=1; ++a1){
      for(a2=-1; a2<=1; ++a2){
	sem_post(&sem_before_update[(block + a1 + a2*nb_block_1D + nb_block)%nb_block]);
	//Concurrent access problem between those two call
	int value;
	sem_getvalue(&sem_before_update[(block + a1 + a2*nb_block_1D + nb_block)%nb_block], &value);
	if(value == 8){
	  pthread_cond_signal(&cond_before_update[(block + a1 + a2*nb_block_1D + nb_block)%nb_block]);
	}
      }
    }

    //Synchro
    while(1){
      pthread_cond_wait(&cond_before_update[block], &mutex_before_update[block]);
      int sem;
      sem_getvalue(&sem_before_update[block], &sem);
      //The thread needs to check the value of the semaphore in case he has been woken up by mistake (pthread_cond_wait isn't secure)
      if(sem==8){
	//Reset
	sem_destroy(&sem_before_update[block]);
	sem_init(&sem_before_update[block], 0, 0);
	break;
      }
    }

    //Update
    int dummy_alive;
    update_1_block(a->block, a->board, a->ldboard, a->nbngb, a->ldnbngb, &dummy_alive);

    //Post to other
    for(a1=-1; a1<=1; ++a1){
      for(a2=-1; a2<=1; ++a2){
	sem_post(&sem_before_ngb[(block + a1 + a2*nb_block_1D + nb_block)%nb_block]);
	//Concurrent access problem between those two call
	int value;
	sem_getvalue(&sem_before_ngb[(block + a1 + a2*nb_block_1D + nb_block)%nb_block], &value);
	if(value == 8){
	  pthread_cond_signal(&cond_before_ngb[(block + a1 + a2*nb_block_1D + nb_block)%nb_block]);
	}
      }
    }
  }
  return NULL;
}

int main(int argc, char* argv[])
{
  int i, j, loop, num_alive, maxloop;
  int ldboard, ldnbngb;
  double t1, t2;
  double temps;
 
  int *board;
  int *nbngb;
  
  if (argc < 2) {
    maxloop = 10;
  } else if (argc > 2){
    maxloop = atoi(argv[1]);
    BS = atoi(argv[2]);
  } else
    maxloop = atoi(argv[1]);
  num_alive = 0;

  /* Leading dimension of the board array */
  ldboard = BS + 2;
  /* Leading dimension of the neigbour counters array */
  ldnbngb = BS;

  board = malloc( ldboard * ldboard * sizeof(int) );
  nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

  num_alive = generate_initial_board( &(cell(0, 0)), ldboard );

  //output_board( BS+2, &(cell(0, 0)), ldboard, 0 );

  printf("Starting number of living cells = %d\n", num_alive);

  //TODO : pthread_create, + lauch the threads by calling pthread_cond_signal on cond_before_ngb

  t1 = mytimer();

  t2 = mytimer();
  temps = t2 - t1;
  printf("Final number of living cells = %d\n", num_alive);
  printf("time=%.2lf ms\n",(double)temps * 1.e3);

  //TODO : join threads

  //output_board( BS, &(cell(1, 1)), ldboard, maxloop);

  free(board);
  free(nbngb);
  return EXIT_SUCCESS;
}

