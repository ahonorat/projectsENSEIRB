#ifndef __MANIP_LIST_H__
#define __MANIP_LIST_H__

#include "common_types.h"


struct thread_container * pop_from_list(struct thread_list *l);
void add_in_list_tail(struct thread_list *l, struct thread_container *thread);
struct thread_container * get_last_element(struct thread_list *list);


#endif
