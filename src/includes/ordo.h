#ifndef __ORDO_H__
#define __ORDO_H__

#include "manip_list.h"

void add_in_list(struct thread_list *l, struct thread_container *thread);
struct thread_container * chose_next_running_thread(struct thread_list *list);

#endif
