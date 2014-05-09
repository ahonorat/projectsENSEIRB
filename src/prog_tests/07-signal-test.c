#include <stdio.h>
#include <stdlib.h>

#include "signal.h"
#include "thread.h"

static void * thfunc(void* thread){
  
  thread_t t= thread_self();
  printf("je suis le thread %p et je vais envoyer un signal à %p", &t,thread);
  thread_t thread_v = (thread_t)thread;
  thread_kill(thread_v,1);
}

void my_handler (int signum){
  printf("j'ai reçu le signal %d\n",signum);
}

int main(int argc, char **argv){

  thread_t *mon_thread;
  thread_t t= thread_self();
  void *tv = (void *)t;
  
  int r = thread_create(mon_thread, &thfunc,t);
  
  
  printf("je suis le thread %p",&t);
  r = thread_sigaction(1,&my_handler,NULL);
  

  return 1;

}
