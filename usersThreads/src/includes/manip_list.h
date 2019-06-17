#ifndef __MANIP_LIST_H__
#define __MANIP_LIST_H__

#include "common_types.h"


struct thread * pop_from_list(struct thread_list *l);
void add_in_list_head(struct thread_list *l, struct thread *thread);
void add_in_list_tail(struct thread_list *l, struct thread *thread);
int exist_thread(struct thread_list *l, struct thread *thread);


#endif
