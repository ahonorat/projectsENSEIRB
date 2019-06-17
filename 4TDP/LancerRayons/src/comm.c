#include "comm.h"
#include "definitions.h"

void* comm(void* arguments){
  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  while(){
    MPI_Status status;
    
    //Blocking call that returns as soon as a matching message has arrived
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
    //
    //Reception of the message
    //
    MPI_Request request;
    //The content of the message is always an int (the rank of the source OR a tile number)
    int buffer;
    MPI_IRecv(&buffer, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &request);
    switch(status.MPI_TAG){

      //Message asking for another tile. Here, the content of the message is an int corresponding to the original source of the message.
    case ASK :

      //If the message originates from the process himself, that means nobody has tasks left for sharing. The process will thus start the ending process.
      if(buffer == myrank){
	//MPI_Isend(NULL, 0, );
      }
      break;
    }
  }
}

