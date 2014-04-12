#include "manip_list.h"


struct thread_container * pop_from_list(struct thread_list *l)
{
  l->num_children--;
  return list_pop(&l->children, struct thread_container, node); 
}


void add_in_list_tail(struct thread_list *l, struct thread_container *thread){
    l->num_children++;
    list_add_tail(&l->children, &thread->node);
}


