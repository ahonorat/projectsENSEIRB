/***************************************************************************
 *                       Lanceur de rayons simplifie                       *
 ***************************************************************************
 * ENSERB, 3me annee Informatique                               1995-1996  *
 * Option AAMP, Module MP2                                                 *
 *                                                                         *
 ***************************************************************************
 * Author : Gilles Subrenat                                                *
 * Date   : 15 janvier 1996                                                *
 *                                                                         *
 * $Id$
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>
#include <semaphore.h>
#include "list.h"
#include "types_tdp.h"

#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"

typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;


MPI_Datatype task_type_todo, task_type_done, color_type;
struct task_list todo_list;
struct task_list done_list;
pthread_mutex_t mutex_todo = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_done = PTHREAD_MUTEX_INITIALIZER;
sem_t number_of_task;
int end = 0;
//The communications thread consist of an infinite loop performing two tasks at each iteration :
//->Managing the incoming message
//->Checking if the todo_list is empty, and if so, send an ASK message
//The problem is that when the todo_list is empty, it will send an ASK message at each iteration. Thus, we use a boolean (asking), to know wether we have already sent an ASK message or not, so that we can avoid sending multiple ones at a time
int asking = 0;
int finishing = 0;
int rank;

static IMG_BASIC  Img;



BOOL
file_img (FILE *File)
{
  GET_COUPLE (Img.Pixel);
  return (TRUE);
}


static COLOR
pixel_basic (INDEX i, INDEX j)
{
  RAY    Ray;
  HIT    Hit;
  REAL   u, v;

  Ray.Level = 1;
  Ray.Color = White;
  u = 2.0 * i / Img.Pixel.i - 1.0;
  v = 2.0 * j / Img.Pixel.j - 1.0;
  cmr (&Ray, u, v);

  if (! hit_bnd (&Ray, &Hit))
    return (MainScn->Atm);
  get_scn (&Ray, &Hit);

  return (Ray.Color);
}



void*
thread_compute(void* arg)
{
  int i,j,k,l;
  sem_wait(&number_of_task);
  //printf("#%d Computation begun\n", rank);
  while(1){
    struct task * task_to_compute;
    // task_todo pop
    pthread_mutex_lock(&mutex_todo);
    if(!list_empty(&(todo_list.children))){
      task_to_compute = list_pop(&todo_list.children, struct task, list);
      --todo_list.num_children;
      pthread_mutex_unlock(&mutex_todo);
      //printf("#%d Task being computed\n", rank);
      // task compute
      for (i = task_to_compute->i, k = 0; i <= task_to_compute->end_i; i++, k++){
	for (j = task_to_compute->j, l = 0; j <= task_to_compute->end_j; j++, l++){
	  task_to_compute->color[k*TILE_SIZE+l] = pixel_basic (j, i);
	  //printf("#%d i: %d, j: %d, r: %e, g: %e, b: %e\n", rank, i, j, task_to_compute->color[k*TILE_SIZE+l].r, task_to_compute->color[k*TILE_SIZE+l].g, task_to_compute->color[k*TILE_SIZE+l].b);
	}
      }
      // task_done add
      pthread_mutex_lock(&mutex_done);
      list_add(&done_list.children, &task_to_compute->list);
      ++done_list.num_children;
      pthread_mutex_unlock(&mutex_done);
    } else { 
      pthread_mutex_unlock(&mutex_todo);
      if(end){
	//printf("#%d Thread exit\n", rank);
	pthread_exit(NULL);
      }
      //printf("#%d List empty\n", rank);
      sem_wait(&number_of_task);
    }
  }
  
}


void *
thread_comm(void* arg)
{
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //printf("#%d Communication begun\n", rank);  
  while(1){
    int flag;
    MPI_Status status_p;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status_p);
    if(flag){
      int content;
      int k,i,length = 1;
      int sem;
      switch(status_p.MPI_TAG){
	//Message asking for another tile. Here, the content of the message is an int corresponding to the original source of the message.
      case ASK :
	//printf("#%d Ask message received\n", rank);
	//Reception of the message
	MPI_Recv(&content, 1, MPI_INT, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//If the message originates from the process himself, that means nobody has tasks left for sharing. The process will thus start the ending process.
	if(content == rank){
	  //actually only the tag is important
	  finishing = 1;
	  MPI_Send(&rank, 1, MPI_INT, (rank+1)%size, END, MPI_COMM_WORLD);
	  //printf("#%d End message sent to %d\n", rank,(rank+1)%size);
	}
	else{
	  //If there is no work left, pass the message
	  pthread_mutex_lock(&mutex_todo);
	  if(list_empty(&(todo_list.children))){
	    pthread_mutex_unlock(&mutex_todo);
	    MPI_Send(&content, 1, MPI_INT, (rank+1)%size, ASK, MPI_COMM_WORLD);
	  }
	  //If there is some work left, pops a tile from the pile and sends it to the asker
	  else{
	    // this if statement doesn't work very well, it seems more complicated to manage semaphores
	    // sometimes, a few tiles may not be computed ... and we don't know why
	    // if length = 1, no problem
	    if (todo_list.num_children > 2*NB_THREADS){ //there is plenty of tasks
	      length = NB_THREADS; //so we send as many tasks as threads
	      //printf("#%d There is plenty of tasks\n", rank);
	    }
	    struct task message[length];
	    for(k = 0; k < length ; k++){
	      struct task * task_to_compute;
	      task_to_compute = list_pop(&todo_list.children, struct task, list);
	      --todo_list.num_children;
	      message[k] = *task_to_compute;
	    }
	    pthread_mutex_unlock(&mutex_todo);
	    MPI_Send(&message[0], length, task_type_todo, content, TILE, MPI_COMM_WORLD);
	  }
	}
	break;
	//Message signifying that nobody has any task left, and that the program his reaching his end
      case END :
	//printf("#%d End message received\n", rank);
	//Reception of the messages
	MPI_Recv(&content, 1, MPI_INT, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//If end equals 0, then it's the first end message we receive. That means that the ending message still haven't gone through the whole circle, and that some processes potentially haven't received the message. Thus, the process will pass it to its successor
	if(0 != rank){
	  //printf("#%d End message from %d transmitted\n", rank, content);
	  MPI_Send(&content, 1, MPI_INT, (rank+1)%size, END, MPI_COMM_WORLD);
	}
	end = 1;
	//Wakes up all the working threads so that they can call pthread_exit()
	/* int i; */
	/* for(i=0;i<NB_THREADS;++i) */
	sem_getvalue(&number_of_task, &sem);
	for(k = sem; k<NB_THREADS; k++)
	  sem_post(&number_of_task);
	//pthread_exit(NULL);
	if (rank != 0){
	  //MPI_Recv(&content, 1, MPI_INT, 0, FINISH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else {
	  /* for (i = 1; i < size; ++i){ */
	  MPI_Send(&rank, 1, MPI_INT, (rank+1)%size, FINISH, MPI_COMM_WORLD); // 1 -> i
	  /* } */
	  return NULL;
	}
	break;
      case FINISH :
	MPI_Recv(&content, 1, MPI_INT, status_p.MPI_SOURCE, FINISH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("#%d Finish message transmitted\n", rank);
	MPI_Send(&content, 1, MPI_INT, (rank+1)%size, FINISH, MPI_COMM_WORLD);
	end = 1;
	//Wakes up all the working threads so that they can call pthread_exit()
	/* int i; */
	/* for(i=0;i<NB_THREADS;++i) */
	sem_getvalue(&number_of_task, &sem);
	for(k = sem; k<NB_THREADS; k++)
	  sem_post(&number_of_task);
	//pthread_exit(NULL);
	return NULL;
	break;
	//The process received a tile number he asked for. The content of the message is the tile number
      case TILE :
	//printf("#%d Tile message received\n", rank);
	//Reception of the message
	MPI_Get_count(&status_p, task_type_todo, &length);
	struct task message[length];
	MPI_Recv(&message, length, task_type_todo, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	asking = 0;
	pthread_mutex_lock(&mutex_todo);
	for(k = 0; k < length ; k++){
	  struct task * task_to_compute = malloc(sizeof(struct task));
	  *task_to_compute = message[k];
	  list_add(&todo_list.children, &task_to_compute->list);
	  ++todo_list.num_children;
	  //printf("#%d Received i: %d, j: %d, ei: %d, ej: %d\n", rank, task_to_compute->i, task_to_compute->j, task_to_compute->end_i, task_to_compute->end_j);
	}
	sem_getvalue(&number_of_task, &sem);
	for(k = sem; k<NB_THREADS; k++)
	  sem_post(&number_of_task);
	pthread_mutex_unlock(&mutex_todo);
	break;
      }
    }
    // The process didn't receive anything, so you check if the process need other tasks
    else{
      if(!finishing){
	pthread_mutex_lock(&mutex_todo);
	if(list_empty(&(todo_list.children))){
	  pthread_mutex_unlock(&mutex_todo);
	  if(!asking){
	    //printf("#%d Ask message sent\n", rank);
	    //Send an ASK message
	    MPI_Send(&rank, 1, MPI_INT, (rank+1)%size, ASK, MPI_COMM_WORLD);
	    asking = 1;
	  }
	}
	else{
	  pthread_mutex_unlock(&mutex_todo);
	}
      }
    }
    //No call to MPI_Wait() is needed, since we don't need to wait for the message to be sent to continue.
  }
}

void
img (const char *FileNameImg)
{
  FILE   *FileImg;   
  COLOR	 *TabColor, *Color;
  STRING Name;
  INDEX	 i, j;
  BYTE   Byte;

  int k, l;
  int m ,n;
  pthread_t threads[NB_THREADS];
  int res_init;
  int size;
  MPI_Init_thread(NULL,NULL,MPI_THREAD_FUNNELED, &res_init);
  if (res_init != MPI_THREAD_FUNNELED){
    //printf("#%d Exiting process.\n", rank);
    MPI_Finalize();
    return ;
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  list_head_init(&todo_list.children);
  todo_list.num_children = 0;
  list_head_init(&done_list.children);
  done_list.num_children = 0;  

  //Here are tiles' types initialization
  //one for color, another for todo_task, the last one for done_task (this one contains the COLOR value) 
  int a_of_b[3] = {1,1,1};
  MPI_Aint a_of_d[3];
  MPI_Datatype a_of_t[3] = {MPI_FLOAT,MPI_FLOAT, MPI_FLOAT};
  MPI_Aint i1,i2;
  COLOR useless_color;
  MPI_Get_address(&useless_color, &i1);
  MPI_Get_address(&useless_color.r, &i2); a_of_d[0] = i2-i1 ;
  MPI_Get_address(&useless_color.g, &i2); a_of_d[1] = i2-i1 ;
  MPI_Get_address(&useless_color.b, &i2); a_of_d[2] = i2-i1 ;
  MPI_Type_create_struct(3, a_of_b, a_of_d, a_of_t,&color_type);
  MPI_Type_commit(&color_type); // this type represent color structure

  int a_of_b1[4] = {1,1,1,1};
  MPI_Aint a_of_d1[4];
  MPI_Datatype a_of_t1[4] = {MPI_INT,MPI_INT,MPI_INT,MPI_INT};
  MPI_Datatype tmp_type;
  MPI_Aint extent;
  struct task useless_task[2];
  MPI_Get_address(&useless_task[0], &i1);
  MPI_Get_address(&useless_task[0].i, &i2); a_of_d1[0] = i2-i1 ;
  MPI_Get_address(&useless_task[0].j, &i2); a_of_d1[1] = i2-i1 ;
  MPI_Get_address(&useless_task[0].end_i, &i2); a_of_d1[2] = i2-i1 ;
  MPI_Get_address(&useless_task[0].end_j, &i2); a_of_d1[3] = i2-i1 ;
  MPI_Get_address(&useless_task[1], &i2); extent = i2-i1 ;
  MPI_Type_create_struct(4, a_of_b1, a_of_d1, a_of_t1,&tmp_type);
  MPI_Type_create_resized(tmp_type, 0, extent, &task_type_todo);
  MPI_Type_commit(&task_type_todo); // this type can represent the first pixel location of a tile or just a pixel

  int a_of_b2[5] = {1,1,1,1,TILE_SIZE*TILE_SIZE};
  MPI_Aint a_of_d2[5];
  MPI_Datatype a_of_t2[5] = {MPI_INT,MPI_INT,MPI_INT,MPI_INT,color_type};
  MPI_Get_address(&useless_task[0], &i1);
  MPI_Get_address(&useless_task[0].i, &i2); a_of_d2[0] = i2-i1 ;
  MPI_Get_address(&useless_task[0].j, &i2); a_of_d2[1] = i2-i1 ;  
  MPI_Get_address(&useless_task[0].end_i, &i2); a_of_d2[2] = i2-i1 ;
  MPI_Get_address(&useless_task[0].end_j, &i2); a_of_d2[3] = i2-i1 ;
  MPI_Get_address(&useless_task[0].color, &i2); a_of_d2[4] = i2-i1 ;
  MPI_Type_create_struct(5, a_of_b2, a_of_d2, a_of_t2,&task_type_done);
  MPI_Type_commit(&task_type_done); // this type represent a computed pixel

  int nb_tile_i = (Img.Pixel.i + TILE_SIZE - 1) / TILE_SIZE;
  int nb_tile_j = (Img.Pixel.j + TILE_SIZE - 1) / TILE_SIZE;
  int nb_tile = nb_tile_i * nb_tile_j;
  int nb_tile_proc_sup = (nb_tile + size - 1) / size;

  if (rank == 0){
    strcpy (Name, FileNameImg);
    strcat (Name, ".ppm");
    INIT_FILE (FileImg, Name, "w");
    fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  }

  // chinese remainder theorem
  int begin, end, int1, int2;
  begin = rank*nb_tile_proc_sup;
  int1 = (rank + 1)*nb_tile_proc_sup - 1;
  int2 = nb_tile - 1;
  end = (int1<int2) ? int1 : int2;
  for(k = begin ; k <= end ; k++){
    int tile = (k*PRIME_NUMBER) % nb_tile;
    struct task * task_to_compute = malloc(sizeof(struct task));
    task_to_compute->i = (tile / nb_tile_j) * TILE_SIZE;
    task_to_compute->j = (tile % nb_tile_j) * TILE_SIZE;
    task_to_compute->end_i = ((task_to_compute->i + TILE_SIZE - 1)<Img.Pixel.i)?(task_to_compute->i + TILE_SIZE - 1):(Img.Pixel.i - 1);
    task_to_compute->end_j = ((task_to_compute->j + TILE_SIZE - 1)<Img.Pixel.j)?(task_to_compute->j + TILE_SIZE - 1):(Img.Pixel.j - 1);
    list_add(&todo_list.children, &task_to_compute->list);
    ++todo_list.num_children;
  }


  sem_init(&number_of_task, 0, NB_THREADS);
  for (k = 0; k<NB_THREADS; k++)
    pthread_create(&threads[k],NULL,thread_compute,NULL);  
  
  printf("#%d Threads created\n", rank);

  thread_comm(NULL);

  for (k = 0; k<NB_THREADS; k++)
    pthread_join(threads[k],NULL);

  printf("#%d All threads joined\n", rank);

  if (rank == 0){
    int proc_tiles_counts[size-1];
    for (k = 1 ; k<size ; k++){
      MPI_Recv(&proc_tiles_counts[k-1], 1, MPI_INT, k, FINISH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    INIT_MEM (TabColor, Img.Pixel.i*Img.Pixel.j, COLOR);
    for (m = 1 ; m<size ; m++){
      for (n = 0 ; n < proc_tiles_counts[m-1] ; n++){
	struct task t;
	MPI_Recv(&t, 1, task_type_done, m, FINISH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	for (i = t.i, k = 0 ; i <= t.end_i ; i++, k++){
	  for (j = t.j, l = 0 ; j <= t.end_j ; j++, l++){
	    TabColor[i*Img.Pixel.j+j] = t.color[(k*TILE_SIZE)+l]; 
	  }
	}
      }
    }
    struct task* t;
    list_for_each(&done_list.children, t, list){
      for (i = t->i, k = 0; i <= t->end_i ; i++, k++){
	for (j = t->j, l = 0 ; j <= t->end_j ; j++, l++){
	  TabColor[i*Img.Pixel.j+j] = t->color[(k*TILE_SIZE)+l];
	}
      }
    }
    
    // the loop in commentar is equivalent, originally Color++ was
    // done with i++ and j++, but it's an error since the incrementation
    // is made one more time than necessary in this case
    // it's a really basic error, and this paragraph is here to remember it
    Color = TabColor;
    for (k = 0; k < Img.Pixel.i*Img.Pixel.j; k++, Color++) {
    /* for (i = 0; i < Img.Pixel.i; i++){ //i++,Color++*/
    /*   for (j = 0; j < Img.Pixel.j; j++) //j++,Color++{ */
    	Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
    	putc (Byte, FileImg);
    	Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
    	putc (Byte, FileImg);
    	Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
    	putc (Byte, FileImg);
	//	printf("%f %f %f ", Color->r, Color->g, Color->b);
      /* 	Color++; */
      /* } */
    }
    fflush (FileImg);  
    EXIT_MEM (TabColor);
    EXIT_FILE (FileImg);
  } else {
    MPI_Send(&done_list.num_children, 1, MPI_INT, 0, FINISH, MPI_COMM_WORLD);
    struct task* t;
    list_for_each(&done_list.children, t, list){
      MPI_Send(t, 1, task_type_done, 0, FINISH, MPI_COMM_WORLD);      
    }
  }
  
  printf("#%d Img rebuild achieved\n", rank);
	

  //free of all tasks, there is no more in todo_list
  while(!list_empty(&(done_list.children))){
    free(list_pop(&done_list.children, struct task, list));
    --done_list.num_children;    
  }

  pthread_mutex_destroy(&mutex_done);
  pthread_mutex_destroy(&mutex_todo);
  sem_destroy(&number_of_task);
  MPI_Finalize();
  
}
