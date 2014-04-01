#include "list.h"


struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};


struct thread_container
{
  struct thread thread;
  struct list_node list;
};

