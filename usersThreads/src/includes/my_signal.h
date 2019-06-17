#ifndef __MY_SIGNAL__
#define __MY_SIGNAL__

#include "manip_list.h"

void add_signal(unsigned int * listsig, int sig);
int check_signal(unsigned int listsig, int sig);
int get_signal(unsigned int listsig);
void signal_done(unsigned int *listsig);

#endif
