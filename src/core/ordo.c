#include "ordo.h"

void add_in_list(struct thread_list *l, struct thread *thread){
  if (thread->adding_type == 0) // FIFO
    add_in_list_tail(l, thread);
  else  //LIFO
    add_in_list_head(l, thread); 
}

struct thread * chose_next_running_thread(struct thread_list * list){
  return pop_from_list(list);
}
