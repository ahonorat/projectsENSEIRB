#include "list.h"
#include "thread.h"
#include <ucontext.h>

struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};


struct thread_container
{
  struct thread thread;
  struct list_node list;
};


struct thread{
  int i;
  ucontext_t uc;
};



extern thread_t thread_self(void){

  ucontext_t uc;
  getcontext(&uc);
  return uc;

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


extern void thread_exit(void *retval) __attribute__ ((__noreturn__)){}

