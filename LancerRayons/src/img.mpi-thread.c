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

#define NUMBER_OF_THREAD 4

typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;


struct task_list todo_list;
struct task_list done_list;
static pthread_mutex_t mutex_todo = PTHREAD_MUTEX_INITIALIZER;
static sem_t number_of_task;
static int end = 0;
//The communications thread consist of an infinite loop performing two tasks at each iteration :
//->Managing the incoming message
//->Checking if the todo_list is empty, and if so, send an ASK message
//The problem is that when the todo_list is empty, it will send an ASK message at each iteration. Thus, we use a boolean (asking), to know wether we have already sent an ASK message or not, so that we can avoid sending multiple ones at a time
static int asking = 0;

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
  while(1){
    sem_wait(&number_of_task);
    if(END == 1){
      pthread_exit(NULL);
    }
    else{
      pthread_mutex_lock(&mutex_todo);
      //TODO : Dépiler une tache et lancer les calculs dessus

      pthread_mutex_unlock(&mutex_todo);      
    }
  }
}


//TODO : il faut détecter quand on a plus de taches dans la liste, afin d'envoyer un message

void* 
thread_comm(void* arg)
{
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  while(1){
    int flag;
    MPI_Status status_p;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status_p);
    
    if(flag){
      //Reception of the message
      //The content of the message is always an int (the rank of the source OR a tile number)
      int content;
      MPI_Recv(&content, 1, MPI_INT, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      switch(status_p.MPI_TAG){
	//Message asking for another tile. Here, the content of the message is an int corresponding to the original source of the message.
      case ASK :
	//If the message originates from the process himself, that means nobody has tasks left for sharing. The process will thus start the ending process.
	if(content == rank){
	  MPI_Request request;
	  //END messages are empty since only the tag is important
	  MPI_Isend(NULL, 0, MPI_INT, (rank+1)%size, END, MPI_COMM_WORLD, &request);
	  //No call to MPI_Wait() is needed, since we don't need to wait for the message to be sent to continue.
	  MPI_Request_free(&request);
	}
	else{
	  //If there is no work left, pass the message
	  pthread_mutex_lock(&mutex_todo);
	  if(list_empty(&(todo_list.children))){
	    pthread_mutex_unlock(&mutex_todo);
	    MPI_Request request;
	    MPI_Isend(&content, 1, MPI_INT, (rank+1)%size, ASK, MPI_COMM_WORLD, &request);
	    MPI_Request_free(&request);
	  }
	  //If there is some work left, pops a tile number from the pile and sends it to the asker
	  else{
	    int message;
	    //TODO : Il faut pop un numéro de tuile dans la todo_list, et placer son contenu dans la variable message

	    MPI_Request request;
	    MPI_Isend(&message, 1, MPI_INT, content, TILE, MPI_COMM_WORLD, &request);
	    pthread_mutex_unlock(&mutex_todo);
	    MPI_Request_free(&request);
	  }
	}
	break;
	//Message signifying that nobody has any task left, and that the program his reaching his end
      case END :
	//If end equals 0, then it's the first end message we receive. That means that the ending message still haven't gone through the whole circle, and that some processes potentially haven't received the message. Thus, the process will pass it to its successor
	if(end == 0){
	  MPI_Request request;
	  MPI_Isend(NULL, 0, MPI_INT, (rank+1)%size, END, MPI_COMM_WORLD, &request);
	  MPI_Request_free(&request);
	}
	end = 1;
	//Wakes up all the working threads so that they can call pthread_exit()
	int i;
	for(i=0;i<NUMBER_OF_THREAD;++i){
	  sem_post(&number_of_task);
	}
	pthread_exit(NULL);
	break;
	//The process received a tile number he asked for. The content of the message is the tile number
      case TILE_NUMBER :
	asking = 0;
	pthread_mutex_lock(&mutex_todo);
	//TODO : Ajouter une tile à la liste. Le numéro de tile est accessible dans la variable content

	sem_post(&number_of_task);
	pthread_mutex_unlock(&mutex_todo);
	break;
      }
    }
    else{
      pthread_mutex_lock(&mutex_todo);
      if(list_empty(&(todo_list.children))){
	pthread_mutex_unlock(&mutex_todo);
	if(!asking){
	  //Send an ASK message
	  int content = rank;
	  MPI_Request request;
	  MPI_Isend(&content, 1, MPI_INT, (rank+1)%size, ASK, MPI_COMM_WORLD, &request);
	  MPI_Request_free(&request);
	  asking = 1;
	}
      }
      else{
	pthread_mutex_unlock(&mutex_todo);
      }
    }
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
  pthread_t threads[NB_THREADS+1];
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

  if (rank == 0){
    strcpy (Name, FileNameImg);
    strcat (Name, ".ppm");
    INIT_FILE (FileImg, Name, "w");
    fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
    //ici calcul et envoi de tous les carreaux initiaux sur chaque proc
  } else {
    //réception et ajout à la pile des tâches à faire 
  }

  //TODO : chaque processus doit initialiser sa sémaphore avec le nombre de taches initialement reçus, en appelant la ligne ci-dessous :
  //sem_init(&number_of_task, 0, <nombre_de_taches>);

  pthread_create(&threads[0],NULL,thread_comm,NULL);  
  for (k = 1; k<=NB_THREADS; k++)
    pthread_create(&threads[k],NULL,thread_compute,NULL);  

  for (k = 0; k<=NB_THREADS; k++)
    pthread_join(threads[k],NULL);

  
  if (rank == 0){
    //+ réception de tous les carreaux calculés
    INIT_MEM (TabColor, Img.Pixel.i, COLOR);
    // attention il faut recréer un TabColor par j, ordre à prendre en compte
    for (j = 0; j < Img.Pixel.j; j++) {
      for (i = 0, Color = TabColor; i < Img.Pixel.i; i++, Color++) {
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
    //envoi de tous les carreaux calculés
  }
  //TODO : si on est un processus quelconque, attendre que tous les workers soient endormis, puis envoyer toutes les tuiles terminées au processus root
  //TODO : si on est le processus root, réceptionner toutes les tuiles

  pthread_mutex_destroy(&mutex_todo);
  sem_destroy(&number_of_task);
  MPI_Finalize();
  
}
