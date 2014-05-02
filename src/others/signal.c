#include "signal.h"

void add_signal(unsigned int * listsig, int sig){
	int mask = 1;
	listsig = listsig | (mask << sig);
}

int thread_kill(thread_t thread,int sig){
	if(priority_level(thread->signal) <= sig)
		thread->signal = sig;
		return 1;
}

int thread_sigaction(int signum,void (*new_sa_handler)(int),void (*old_sa_handler)(int));
  if(new_sa_handler)
    new_sa_handler(signum); 
  thread->signal = 0;

  return 1;

}

