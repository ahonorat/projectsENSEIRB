#ifndef __TYPES_TDP_H__
#define __TYPES_TDP_H__

#include "type.h"
#include "list.h"

#define NB_THREADS 2

enum tag {ASK, END, TILE};

struct task_list
{
  struct list_head children;
  unsigned int num_children;
};


struct task
{
  struct list_node list;
  INDEX i,j; //int
  COLOR color; //3*float
};


#endif
