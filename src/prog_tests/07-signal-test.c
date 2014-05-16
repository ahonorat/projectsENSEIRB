#include <stdio.h>
#include <stdlib.h>

#include "thread.h"

static void * thfunc(void* thread){

  thread_t t= thread_self();
  thread_t thread_v = (thread_t) thread;
  printf("je suis le thread %p et je vais envoyer le signal %d à %p\n", (void *) &t,SIGILL,thread);
  thread_kill(thread_v,SIGILL);
  printf("je suis le thread %p et je vais envoyer le signal %d à %p\n", (void *) &t,SIGFPE,thread);
  thread_kill(thread_v,SIGFPE);
  thread_yield();
  return thread;
}

void my_handler (int signum){
  printf("je suis vraiment le thread %p et j'ai reçu le signal %d\n",(void *) thread_self(),signum);
}

int main(){

  thread_t mon_thread1, mon_thread2;
  thread_t t = thread_self();
  int r;

  printf("je suis le thread chef :: %p\n",(void *) t);
  r = thread_create(&mon_thread1, &thfunc,(void *) t);
  thread_signal(SIGILL,&my_handler);
  thread_signal(SIGFPE,&my_handler);
  r = thread_create(&mon_thread2, &thfunc,(void *) t);

  
  if(r<0){
    perror("erreur thread_create");
    return -1;
  }
      
  printf("je suis le thread %p et je suis de retour dans le main\n",(void *) t);
  
  if(r<0){
    perror("erreur thread_signal");
    return -1;
  }

  return 0;
}
