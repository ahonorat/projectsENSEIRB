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


struct task_list todo_list;
struct task_list done_list;

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
  
  pthread_exit(NULL);
}


void* 
thread_comm(void* arg)
{
  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  while(1){
    MPI_Status status_p, status_r;
    
    //Blocking call that returns as soon as a matching message has arrived
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status_p);
    
    //
    //Reception of the message
    //
    //The content of the message is always an int (the rank of the source OR a tile number)
    int buffer;
    MPI_Recv(&buffer, 1, MPI_INT, status_p.MPI_SOURCE, status_p.MPI_TAG, MPI_COMM_WORLD, &status_r);
    switch(status_p.MPI_TAG){

      //Message asking for another tile. Here, the content of the message is an int corresponding to the original source of the message.
    case ASK :

      //If the message originates from the process himself, that means nobody has tasks left for sharing. The process will thus start the ending process.
      if(buffer == myrank){
	//MPI_Isend(NULL, 0, );
      }
      break;
    }
  }
  pthread_exit(NULL);
}


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

  MPI_Finalize();
  
}
