#include "common_types.h"
#include "my_signal.h"


void rm_signal(unsigned int * listsig, int sig){  
	int mask=1;
	*listsig=*listsig & !(mask << sig);
}

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
  if(check_signal(listsig,THREAD_SIGKILL))
    return THREAD_SIGKILL;
  if(check_signal(listsig,THREAD_SIGKILL))
    return THREAD_SIGSTOP;
  int i,k = 0;

  for(i = 1; i < 23; i++){
    if(check_signal(listsig,i)){
      k = i;
      if ((k == THREAD_SIGUSR1) || (k == THREAD_SIGUSR2)) // traitement des sig user après les autres
        k = 0;
      if (k != 0)
        return k;
    }
  }
  if(check_signal(listsig,THREAD_SIGUSR1))
    return THREAD_SIGUSR1;
  if(check_signal(listsig,THREAD_SIGUSR2))
    return THREAD_SIGUSR2;

  return 0;
}

void signal_done(unsigned int *listsig){
  *listsig = 0;
}

