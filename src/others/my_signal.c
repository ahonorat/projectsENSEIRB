#include "my_signal.h"

void add_signal(unsigned int * listsig, int sig){
	int mask=1;
	*listsig=*listsig | (mask << sig);
}

int check_signal(unsigned int listsig, int sig){
	int mask=1;
	int res=listsig & (mask << sig);
	return res;
}

int get_signal(unsigned int listsig){
  int i=0;
  for(i=0;i<19;i++){
    if(check_signal(listsig,i)){
      return i;
    }
  }
  //aucun signal, le thread a -1
  return -1;
}
void signal_done(unsigned int *listsig){
  *listsig = 0;
}

int thread_kill(thread_t thread,int sig){
  add_signal(&thread->signal, sig);
  return 1;
}

int thread_sigaction(int signum,void (*new_sa_handler)(int),void (*old_sa_handler)(int)){
  struct thread *thread = thread_self();
  
  if(get_signal(thread->signal) == signum){
    if(new_sa_handler)
      new_sa_handler(signum); 
    signal_done(&thread->signal);
  }
  return 1;
}

