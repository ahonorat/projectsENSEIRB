#ifndef __TYPES_TDP_H__
#define __TYPES_TDP_H__

#include "type.h"
#include "list.h"

#define NB_THREADS 2

#define TILE_SIZE 16

// why not ? it should be large enough ...
#define PRIME_NUMBER 10111

enum tag {ASK, END, TILE, INIT, FINISH};

struct task_list
{
  struct list_head children;
  unsigned int num_children;
};


struct task
{
  struct list_node list;
  INDEX i,j; //int
  INDEX end_i, end_j; //int (usefull if last blocks are smaller)
  COLOR color[TILE_SIZE*TILE_SIZE]; //TILE_SIZE*TILE_SIZE*3*float ; ROW MAJOR
};


#endif
