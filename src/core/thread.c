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
  struct list_node node;
  struct thread *thread;
};

/*Tout doux*/
inline struct thread_container * pop_from_list(struct thread_list *l);
inline void add_in_list(struct thread_list *l, struct thread_container *thread){}
inline struct thread_container * get_last_element(struct thread_list *list){}
/*/todo*/
void run_thread(struct thread_container * next_running_thread);
static struct thread_list ready_list;
static struct thread_list waiting_list;

static struct thread_container *running;


inline struct thread_container * chose_next_running_thread()
{
  return get_last_element(&ready_list);
}

static void thread_init() __attribute__ ((constructor));
static void thread_init()
{
  list_head_init(&ready_list.children);
  ready_list.num_children = 0;
  running = (struct thread_container *)malloc(sizeof(struct thread_container));
  running->thread = (struct thread *)malloc(sizeof(struct thread));
}

void thread_quit() __attribute__ ((destructor));
void thread_quit(){
  if(running->thread)
    free(running->thread);
  
  // question a faverge pour libérer le thread main et les autres  
  
}


extern thread_t thread_self(void){
  return running->thread;
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
  makecontext(&uc, (void (*)(void)) func, 1, funcarg);

  //allocation newthread
  *newthread = (thread_t) malloc(sizeof(struct thread));
  if(!newthread){
    free(uc.uc_stack.ss_sp);
    perror("malloc newthread ds thread_create");
    return 1;
  }
#ifndef NDEBUG
  (*newthread)->valgrind_stackid = VALGRIND_STACK_REGISTER(uc.uc_stack.ss_sp,uc.uc_stack.ss_sp + uc.uc_stack.ss_size);
#endif
  
  // initialisation newthread
  (*newthread)->uc = uc;
  (*newthread)->retval = NULL;
  struct thread_container *c = (struct thread_container*) malloc(sizeof(struct thread_container));
  c->thread = *newthread;
  list_add_tail(&ready_list.children, &c->node);
  
  // appel de yield
  if (thread_yield()){
    perror("erreur appel yield ds thread_create");
    return 1;
  }
  return 0;
}



extern int thread_yield(void){
  //place le thread courant dans la liste des threads ready
  list_add_tail(&ready_list.children, &running->node);
  struct thread_container * top = pop_from_list(&ready_list);
  swapcontext(&running->thread->uc, &top->thread->uc);
  running = top;
  return 0;
}


extern int thread_join(thread_t thread, void **retval){
  
  return 0;
}


extern void thread_exit(void *retval) {
  running->thread->retval = retval;
  add_in_list(&waiting_list, running);
  run_thread(chose_next_running_thread());
}

void run_thread(struct thread_container * next_running_thread)
{
  if(next_running_thread == NULL)
    exit(0);
  running = next_running_thread;
  setcontext(&next_running_thread->thread->uc);
}

inline struct thread_container * pop_from_list(struct thread_list *l)
{
  return list_pop(&l->children, struct thread_container, node); 
}
