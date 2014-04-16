#include "ordo.h"
#include "thread.h"

#define SIZE_THREAD 64*1024

/* Prototypes des variables globales et fonctions locales */
int thread_construct(struct thread *th, int is_main);
void run_thread(struct thread * next_running_thread);
void * th_intermediare(void * res);
static void thread_init() __attribute__ ((constructor));
static void thread_quit() __attribute__ ((destructor));

static struct thread_list ready_list;
static struct thread_list waiting_list;
static ucontext_t exiting_context;

static struct thread *running;

/* Définitions des fonctions locales */
void thread_destruct(struct thread * th){
  if(running != th)
    list_del(&th->node);
  if(!th -> is_main){
    free(th -> uc . uc_stack . ss_sp);
  }
}

int thread_construct(struct thread *th, int is_main){
  th->status = READY;
  th -> is_main = is_main;
  ucontext_t * uc = &th -> uc;
  if(!is_main){
    uc->uc_stack.ss_size = SIZE_THREAD;
    uc->uc_stack.ss_sp = malloc(uc->uc_stack.ss_size);
    if(!uc->uc_stack.ss_sp){
      perror("malloc stack ds thread_create");
      return -1;
    }
#ifndef NDEBUG
    th->valgrind_stackid = VALGRIND_STACK_REGISTER(uc->uc_stack.ss_sp,uc->uc_stack.ss_sp + uc->uc_stack.ss_size);
#endif
		
  }
  uc->uc_link = NULL;
	
  getcontext(&th->uc);
  return 0;
}

void run_thread(struct thread * next_running_thread)
{
  if(next_running_thread == NULL)
    setcontext(&exiting_context);
  running = next_running_thread;
  setcontext(&next_running_thread->uc);
}

void * th_intermediaire(void *(*func)(void *), void *funcarg){
  void * res = func(funcarg);
  thread_exit(res);
  assert(0); //on n'est pas censé arriver là
  return NULL;
}

static void thread_init()
{
  list_head_init(&ready_list.children);
  ready_list.num_children = 0;
  list_head_init(&waiting_list.children);
  waiting_list.num_children = 0;
  running = (struct thread *)malloc(sizeof(struct thread));
  thread_construct(running, 1);
}

static void thread_quit(){
  if(running){
    thread_destruct(running);
    free(running);
  }  
}


#ifndef USE_PTHREAD

/* Fonctions de la bibliothèque */


extern struct thread * thread_self(void){
  return running;
}


extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
 
  ucontext_t * uc;

  //allocation newthread
  *newthread = (struct thread *) malloc(sizeof(struct thread));
  if(!newthread){
    perror("malloc newthread ds thread_create");
    return -1;
  }
 
  // initialisation newthread
  uc = &(*newthread)->uc;
  if(thread_construct(*newthread, 0)==-1){
    free(newthread);
    return -1;
  }
  makecontext(uc, (void (*)(void)) th_intermediaire, 2, func, funcarg);


  (*newthread)->retval = NULL;

  add_in_list(&ready_list, *newthread);

  // appel de yield
  if (thread_yield()){
    perror("erreur appel yield ds thread_create");
    return -1;
  }
  return 0;
}



extern int thread_yield(void){
  //place le thread courant dans la liste des threads ready
  add_in_list(&ready_list, running);
  struct thread * top = pop_from_list(&ready_list);
  struct thread * prev = running;
  running = top;
  return swapcontext(&prev->uc, &top->uc);
}


extern int thread_join(thread_t thread, void **retval){
  while (thread->status != WAITING)
    thread_yield();
  *retval = thread->retval;
    thread_destruct(thread);
    free(thread);
  return 0;
}


extern void thread_exit(void *retval) {
  running->retval = retval;
  running->status = WAITING;
  add_in_list(&waiting_list, running);

  if (running->is_main)
  {
    struct thread * valid_thread = chose_next_running_thread(&ready_list);
    running = valid_thread;
    getcontext(&exiting_context);
    swapcontext(&exiting_context, &valid_thread->uc);
    exit(0);
  }
  else
    run_thread(chose_next_running_thread(&ready_list));
}

#endif
