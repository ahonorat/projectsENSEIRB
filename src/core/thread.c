#include "list.h"
#include "thread.h"
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};

struct thread{
  ucontext_t uc;
};

struct thread_container
{
  struct thread *thread;
  struct list_node list;
};

static struct thread_list readylist;
static thread_t running;

int thread_init() __attribute__ ((constructor));
int thread_init()
{
  list_head_init(&readylist.children);
  readylist.num_children = 0;
  return 1;
}

void thread_quit() __attribute__ ((destructor));
void thread_quit(){
  return;
}


extern thread_t thread_self(void){
  return running;
}


extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
  //  return makecontext(newthread,func,funcarg); 
  return 0;
}


extern int thread_yield(void){
  //placer le thread courant dans la liste des threads ready
  if (!list_empty(readylist.children)){
    
    
    
  }


  //  thread_t curr = 
  
  return 0;
}


extern int thread_join(thread_t thread, void **retval){
  return 0;
}


extern void thread_exit(void *retval) {
  exit(0);
}

