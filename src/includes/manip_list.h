#ifndef __MANIP_LIST_H__
#define __MANIP_LIST_H__

#include "common_types.h"


struct thread * pop_from_list(struct thread_list *l);
void add_in_list_tail(struct thread_list *l, struct thread *thread);


#endif
