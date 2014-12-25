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
  int i,j;
  while(1){
    sem_wait(&number_of_task);
    if(END == 1){
      pthread_exit(NULL);
    }
    else{
      struct task * task_to_compute;
      // task_todo pop
      pthread_mutex_lock(&mutex_todo);
      if(!list_empty(&(todo_list.children))){
	task_to_compute = list_pop(&todo_list.children, struct task, list);
	--todo_list.num_children;
	pthread_mutex_unlock(&mutex_todo);
	// task compute
	for (i = task_to_compute->i; i < task_to_compute->end_i;i++){
	  for (j = task_to_compute->j; j < task_to_compute->end_j;j++){
	    task_to_compute->color[i*TILE_SIZE+j] = pixel_basic (i, j);
	  }
	}
	// task_done add
	pthread_mutex_lock(&mutex_done);
	list_add(&done_list.children, &task_to_compute->list);
	++done_list.num_children;
	pthread_mutex_unlock(&mutex_done);
      } else { 
	pthread_mutex_unlock(&mutex_todo);
      }
    }
  }
}


void *
thread_comm(void* arg)
{
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  while(1){
    int flag;
    MPI_Status status_p;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status_p);
    MPI_Request request;    
    if(flag){
      int content;
      int k,length = 1;	    
      switch(status_p.MPI_TAG){
	//Message asking for another tile. Here, the content of the message is an int corresponding to the original source of the message.
      case ASK :
	//Reception of the message
	MPI_Recv(&content, 1, MPI_INT, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//If the message originates from the process himself, that means nobody has tasks left for sharing. The process will thus start the ending process.
	if(content == rank){
	  //END messages are empty since only the tag is important
	  MPI_Isend(NULL, 0, MPI_INT, (rank+1)%size, END, MPI_COMM_WORLD, &request);
	  MPI_Request_free(&request);
	}
	else{
	  //If there is no work left, pass the message
	  pthread_mutex_lock(&mutex_todo);
	  if(list_empty(&(todo_list.children))){
	    pthread_mutex_unlock(&mutex_todo);
	    MPI_Isend(&content, 1, MPI_INT, (rank+1)%size, ASK, MPI_COMM_WORLD, &request);
	    MPI_Request_free(&request);
	  }
	  //If there is some work left, pops a tile number from the pile and sends it to the asker
	  else{
	    if (todo_list.num_children > 2*NB_THREADS) //there is plenty of tasks
	      length = NB_THREADS; //so we send as many tasks as threads
	    struct task message[length];
	    for(k = 0; k < length ; k++){
	      struct task * task_to_compute;
	      task_to_compute = list_pop(&todo_list.children, struct task, list);
	      --todo_list.num_children;
	      message[k] = *task_to_compute;
	    }
	    pthread_mutex_unlock(&mutex_todo);
	    MPI_Isend(&message, length, task_type_todo, content, TILE, MPI_COMM_WORLD, &request);
	    MPI_Request_free(&request);
	  }
	}
	break;
	//Message signifying that nobody has any task left, and that the program his reaching his end
      case END :
	//Reception of the messages
	MPI_Recv(&content, 1, MPI_INT, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//If end equals 0, then it's the first end message we receive. That means that the ending message still haven't gone through the whole circle, and that some processes potentially haven't received the message. Thus, the process will pass it to its successor
	if(end == 0){
	  MPI_Isend(NULL, 0, MPI_INT, (rank+1)%size, END, MPI_COMM_WORLD, &request);
	  MPI_Request_free(&request);
	}
	end = 1;
	//Wakes up all the working threads so that they can call pthread_exit()
	int i;
	for(i=0;i<NB_THREADS;++i){
	  sem_post(&number_of_task);
	}
	//pthread_exit(NULL);
	return NULL;
	break;
	//The process received a tile number he asked for. The content of the message is the tile number
      case TILE :
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
	  sem_post(&number_of_task);
	}
	pthread_mutex_unlock(&mutex_todo);
	break;
      }
    }
    // The process didn't receive anything, so you check if the process need other tasks
    else{
      if(!end){
	pthread_mutex_lock(&mutex_todo);
	if(list_empty(&(todo_list.children))){
	  pthread_mutex_unlock(&mutex_todo);
	  if(!asking){
	    //Send an ASK message
	    MPI_Isend(&rank, 1, MPI_INT, (rank+1)%size, ASK, MPI_COMM_WORLD, &request);
	    MPI_Request_free(&request);
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

//TODO : Grossièrement, la fonction est à faire en 3 étapes :
//1) Le processus root distribue les numéros de tuiles, les autres réceptionnent (tous les processus ont une copie des données, donc juste besoin d'envoyer un numéro de tuile)
//2) Calcul multi-threadé
//3) Chaque processus envoie ses tuiles au processus root. Le processus root réceptionne le tout.
//Comme l'envoie des tuiles ne se fait qu'à la fin et pas pendant l'étape de calcul, ce n'est pas au thread de comm de gérer ça, et ça simplifie les choses (enfin je crois hé hé hé)
void
img (const char *FileNameImg)
{
  FILE   *FileImg;   
  COLOR	 *TabColor, *Color;
  STRING Name;
  INDEX	 i, j;
  BYTE   Byte;

  int k;
  pthread_t threads[NB_THREADS];
  int res_init;
  int rank, size;
  //ELOUAN : À voir si on peut pas utiliser MPI_THREAD_FUNNELED au lieu de MPI_THREAD_SERIALIZED. Ça correspond à : "Seulement le thread main fait des appels MPI". Dans ces cas-là au lieu de lancer un thread qui appellerait la fonction de communication, on ferait directement appeler la fonction comm par le thread main, et on remplacerait le "pthread_exit()" par un simple "return"
  MPI_Init_thread(NULL,NULL,MPI_THREAD_SERIALIZED, &res_init);
  if ((res_init != MPI_THREAD_SERIALIZED) && (res_init != MPI_THREAD_MULTIPLE)){
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
  int a_of_b[6] = {1,1,1};
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
  struct task useless_task;
  MPI_Get_address(&useless_task, &i1);
  MPI_Get_address(&useless_task.i, &i2); a_of_d1[0] = i2-i1 ;
  MPI_Get_address(&useless_task.j, &i2); a_of_d1[1] = i2-i1 ;
  MPI_Get_address(&useless_task.end_i, &i2); a_of_d1[2] = i2-i1 ;
  MPI_Get_address(&useless_task.end_j, &i2); a_of_d1[3] = i2-i1 ;
  MPI_Type_create_struct(3, a_of_b1, a_of_d1, a_of_t1,&task_type_todo);
  MPI_Type_commit(&task_type_todo); // this type can represent the first pixel location of a tile or just a pixel

  int a_of_b2[5] = {1,1,1,1,TILE_SIZE*TILE_SIZE};
  MPI_Aint a_of_d2[5];
  MPI_Datatype a_of_t2[5] = {MPI_INT,MPI_INT,MPI_INT,MPI_INT,color_type};
  MPI_Get_address(&useless_task, &i1);
  MPI_Get_address(&useless_task.i, &i2); a_of_d2[0] = i2-i1 ;
  MPI_Get_address(&useless_task.j, &i2); a_of_d2[1] = i2-i1 ;  
  MPI_Get_address(&useless_task.end_i, &i2); a_of_d1[2] = i2-i1 ;
  MPI_Get_address(&useless_task.end_j, &i2); a_of_d1[3] = i2-i1 ;
  MPI_Get_address(&useless_task.color, &i2); a_of_d2[2] = i2-i1 ;
  MPI_Type_create_struct(5, a_of_b2, a_of_d2, a_of_t2,&task_type_done);
  MPI_Type_commit(&task_type_done); // this type represent a computed pixel

  unsigned int nb_tile_i = (Img.Pixel.i + TILE_SIZE - 1) / TILE_SIZE;
  unsigned int nb_tile_j = (Img.Pixel.j + TILE_SIZE - 1) / TILE_SIZE;
  unsigned int nb_tile = nb_tile_i * nb_tile_j;
  unsigned int nb_tile_proc_sup = (nb_tile + size - 1) / size;

  if (rank == 0){
    strcpy (Name, FileNameImg);
    strcat (Name, ".ppm");
    INIT_FILE (FileImg, Name, "w");
    fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  }

  // chinese remainder theorem
  unsigned int begin, end, int1, int2;
  begin = rank*nb_tile_proc_sup;
  int1 = (rank + 1)*nb_tile_proc_sup - 1;
  int2 = nb_tile - 1;
  end = (int1<int2) ? int1 : int2;
  for(k = begin ; k < end ; k++){
    unsigned int tile = (k*PRIME_NUMBER) % nb_tile;
    struct task * task_to_compute = malloc(sizeof(struct task));
    task_to_compute->i = (tile / nb_tile_j) * TILE_SIZE;
    task_to_compute->j = (tile - task_to_compute->i*nb_tile_j) * TILE_SIZE;
    task_to_compute->end_i = ((task_to_compute->i + TILE_SIZE)<Img.Pixel.i)?(task_to_compute->i + TILE_SIZE):(Img.Pixel.i - 1);
    task_to_compute->end_j = ((task_to_compute->j + TILE_SIZE)<Img.Pixel.j)?(task_to_compute->j + TILE_SIZE):(Img.Pixel.j - 1);
    list_add(&todo_list.children, &task_to_compute->list);
    ++todo_list.num_children;
  }


  sem_init(&number_of_task, 0, NB_THREADS);
  for (k = 0; k<NB_THREADS; k++)
    pthread_create(&threads[k],NULL,thread_compute,NULL);  

  thread_comm(NULL);

  for (k = 0; k<NB_THREADS; k++)
    pthread_join(threads[k],NULL);

  
  if (rank == 0){
    int proc_tiles_counts[size-1];
    for (k = 1 ; k<size ; k++){
      MPI_Recv(&proc_tiles_counts[k-1], 1, MPI_INT, k, FINISH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    // attention les indices i <-> sont peut-être inversés
    INIT_MEM (TabColor, Img.Pixel.i*Img.Pixel.j, COLOR);
    for (k = 1 ; k<size ; k++){
      struct task t;
      MPI_Recv(&t, 1, task_type_done, k, FINISH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (i = t.i ; i <= t.end_i ; i++){
	for (j = t.j ; j <= t.end_j ; j++){
	  TabColor[i*Img.Pixel.j+j] = t.color[(i*Img.Pixel.j+j)%TILE_SIZE]; 
	}
      }
    }
    struct task* t;
    list_for_each(&done_list.children, t, list){
      for (i = t->i ; i <= t->end_i ; i++){
	for (j = t->j ; j <= t->end_j ; j++){
	  TabColor[i*Img.Pixel.j+j] = t->color[(i*Img.Pixel.j+j)%TILE_SIZE]; 
	}
      }
    }
    
    Color = TabColor;
    for (j = 0; j < Img.Pixel.j; j++) {
      for (i = 0 ; i < Img.Pixel.i; i++, Color++) {
    	Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
    	putc (Byte, FileImg);
    	Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
    	putc (Byte, FileImg);
    	Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
    	putc (Byte, FileImg);
      }
      fflush (FileImg);
    }
    EXIT_MEM (TabColor);
    EXIT_FILE (FileImg);
  } else {
    MPI_Send(&done_list.num_children, 1, MPI_INT, 0, FINISH, MPI_COMM_WORLD);
    struct task* t;
    list_for_each(&done_list.children, t, list){
      MPI_Send(t, 1, task_type_done, 0, FINISH, MPI_COMM_WORLD);      
    }
  }

  //free of all tasks, there is no more in todo_list
  //TODO : supprimer ce code si on fait la désallocation lors de la lecture des tâches
  while(!list_empty(&(done_list.children))){
    free(list_pop(&todo_list.children, struct task, list));
    --todo_list.num_children;    
  }

  pthread_mutex_destroy(&mutex_done);
  pthread_mutex_destroy(&mutex_todo);
  sem_destroy(&number_of_task);
  MPI_Finalize();
  
}
