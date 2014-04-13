#include "ordo.h"

void add_in_list(struct thread_list *l, struct thread_container *thread)
{
  add_in_list_tail(l, thread);
}

struct thread_container * chose_next_running_thread(struct thread_list * list)
{
  return pop_from_list(list);
}
