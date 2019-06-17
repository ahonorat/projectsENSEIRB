#ifndef __ORDO_H__
#define __ORDO_H__

#include "manip_list.h"

void add_in_list(struct thread_list *l, struct thread *thread);
struct thread * chose_next_running_thread(struct thread_list *list);

#endif
