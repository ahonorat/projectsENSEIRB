#ifndef __MY_SIGNAL_H__
#define __MY_SIGNAL_H__

#include "thread.h"
#include "common_types.h"

int thread_kill(thread_t thread,int sig);

int thread_sigaction(int signum,void (*new_sa_handler)(int),void (*old_sa_handler)(int));

#endif
