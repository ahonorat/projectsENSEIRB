#include "list.h"
#include "thread.h"
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>

struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};
struct thread_container;

struct thread{
  ucontext_t uc;
  void * retval;
#ifndef NDEBUG
  int valgrind_stackid;
#endif
};

struct thread_container
{
  struct list_node list;
  struct thread *thread;
};

static struct thread_list readylist;
static struct thread_list waitinglist;

static struct thread_container *running;

static void thread_init() __attribute__ ((constructor));
static void thread_init()
{
  list_head_init(&readylist.children);
  readylist.num_children = 0;
  running.thread = (struct thread *)malloc(sizeof(struct thread));
}

void thread_quit() __attribute__ ((destructor));
void thread_quit(){
  if(running.thread)
    free(running.thread);
  
  // question a faverge pour libérer le thread main et les autres  
  
}


extern thread_t thread_self(void){
  return running.thread;
}


extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){

  //creation de contexte 
  ucontext_t uc;
  uc.uc_stack.ss_size = 64*1024;
  uc.uc_stack.ss_sp = malloc(uc.uc_stack.ss_size);
  if(!uc.uc_stack.ss_sp){
    perror("malloc stack ds thread_create");
    return 1;
  }
  uc.uc_link = NULL;
  makecontext(&uc,func,funcarg);
  
  //allocation newthread
  newthread = (struct thread *)malloc(sizeof(struct thread));
  if(!newthread){
    free(uc.uc_stack.ss_sp);
    perror("malloc newthread ds thread_create");
    return 1;
  }
#ifndef NDEBUG
  newthread.valgrind_stackid = VALGRIND_STACK_REGISTER(uc.uc_stack.ss_sp,uc.uc_stack.ss_sp + uc.uc_stack.ss_size);
#endif
  
  // initialisation newthread
  newthread->uc = uc;
  newthread->retval = NULL;
  struct thread_container *c = (struct thread_container)malloc(sizeof(struct thread_container));
  c.thread = newthread;
  list_add_tail(&readylist.children, &c.list);
  
  // appel de yield
  if (thread_yield()){
    perror("erreur appel yield ds thread_create");
    return 1;
  }
  return 0;
}



extern int thread_yield(void){
  //place le thread courant dans la liste des threads ready
  struct thread_container c;
  c.thread = running.thread;
  list_add_tail(&readylist.children, &running.list);
  struct thread * top=list_pop_(&readylist.children,0);
  swapcontext(&running.thread->uc, &top.thread->uc);
  running.thread = top;
  return 0;
}


extern int thread_join(thread_t thread, void **retval){
	
  return 0;
}


extern void thread_exit(void *retval) {
  exit(0);
}

