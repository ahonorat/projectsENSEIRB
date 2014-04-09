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
struct thread_container;

struct thread{
  ucontext_t uc;
  void * retval;
};

struct thread_container
{
  struct list_node list;
  struct thread *thread;
};

static struct thread_list readylist;

static struct thread_container running;//TODO modifs

static void thread_init() __attribute__ ((constructor));
static void thread_init()
{
  list_head_init(&readylist.children);
  readylist.num_children = 0;
}

void thread_quit() __attribute__ ((destructor));
void thread_quit(){
  return;
}


extern thread_t thread_self(void){
  //running.thread = malloc(sizeof(struct thread)); // juste pour verifier qu'il renvoie bien le pointeur alloue
  return running.thread;
}


extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
  //  return makecontext(newthread,func,funcarg); 
  return 0;
}


extern int thread_yield(void){
  //placer le thread courant dans la liste des threads ready
  if (!list_empty(&readylist.children)){
    struct thread * top=list_pop_(&readylist.children,0);
    list_add_tail(&readylist.children, &running.list);
    //running.list = NULL;
    running.thread = top;
    
    
  }
  return 0;
}


extern int thread_join(thread_t thread, void **retval){
	
  return 0;
}


extern void thread_exit(void *retval) {
  exit(0);
}

