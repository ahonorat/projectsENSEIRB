#include "list.h"
#include "thread.h"
#include <ucontext.h>
#include <stdlib.h>

struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};

struct thread{
  int i;
  ucontext_t uc;
};

struct thread_container
{
  struct thread thread;
  struct list_node list;
};



extern thread_t thread_self(void){
  ucontext_t uc;
  getcontext(&uc);
  return NULL;
}


extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
  //  makecontext(&uc,func, funcarg);
  return 0;
}


extern int thread_yield(void){
  return 0;
}


extern int thread_join(thread_t thread, void **retval){
  return 0;
}


extern void thread_exit(void *retval) {
  exit(0);
}

