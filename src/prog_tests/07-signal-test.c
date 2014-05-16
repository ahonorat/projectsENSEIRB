#include <stdio.h>
#include <stdlib.h>

#include "thread.h"

static void * thfunc(void* thread){

  thread_t t= thread_self();
  thread_t thread_v = (thread_t) thread;
  printf("je suis le thread %p et je vais envoyer le signal %d à %p\n", (void *) &t,SIGTERM,thread);
  thread_kill(thread_v,SIGTERM);
  
  printf("je suis le thread %p et je vais envoyer le signal %d à %p\n", (void *) &t,SIGKILL,thread);
  thread_kill(thread_v,SIGKILL);
  
  return thread;
}

void my_handler (int signum){
  printf("je suis vraiment le thread %p et j'ai reçu le signal %d\n",(void *) thread_self(),signum);
}

int main(){

  thread_t mon_thread;
  thread_t t = thread_self();
  int r;

  printf("je suis le thread chef :: %p\n",(void *) t);
  r = thread_create(&mon_thread, &thfunc,(void *) t);
  thread_yield();
  
  if(r<0){
    perror("erreur thread_create");
    return -1;
  }
      
  printf("je suis le thread %p et je suis de retour dans le main\n",(void *) t);
  thread_signal(SIGTERM,&my_handler);
  thread_signal(SIGKILL,&my_handler);
  
  if(r<0){
    perror("erreur thread_signal");
    return -1;
  }

  return 0;
}
