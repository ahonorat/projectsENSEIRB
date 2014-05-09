#include <stdio.h>
#include <stdlib.h>

#include "my_signal.h"
#include "thread.h"

static void * thfunc(void* thread){
  thread_t t= thread_self();
  thread_t thread_v = (thread_t)thread;
  printf("je suis le thread %p et je vais envoyer le signal %d à %p\n", &t,THREAD_SIGTERM,thread);
  thread_kill(thread_v,THREAD_SIGTERM);
  
  printf("je suis le thread %p et je vais envoyer le signal %d à %p\n", &t,THREAD_SIGKILL,thread);
  thread_kill(thread_v,THREAD_SIGKILL);

  return thread;
}

void my_handler (int signum){
  printf("je suis vraiment le thread %p et j'ai reçu le signal %d\n",thread_self(),signum);
}

int main(){

  thread_t mon_thread;
  thread_t t= thread_self();
  
  printf("je suis le thread chef :: %p\n",t);
  
  int r = thread_create(&mon_thread, &thfunc,t);
  
  if(r<0){
    perror("erreur thread_create");
    return -1;
  }
      
  printf("je suis le thread %p et je suis de retour dans le main\n",t);
  r = thread_sigaction(THREAD_SIGTERM,&my_handler,NULL);
  r = thread_sigaction(THREAD_SIGKILL,&my_handler,NULL);
  
  if(r<0){
    perror("erreur thread_sigaction");
    return -1;
  }


  return 1;

}
