#include "manip_list.h"


struct thread * pop_from_list(struct thread_list *l){
  l->num_children--;
  return list_pop(&l->children, struct thread, node); 
}


void add_in_list_tail(struct thread_list *l, struct thread *thread){
    l->num_children++;
    list_add_tail(&l->children, &thread->node);
}


