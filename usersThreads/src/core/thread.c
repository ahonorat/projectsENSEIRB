#include <signal.h>

#include "ordo.h"
#include "thread.h"
#include "preemption.h"
#include "my_signal.h"

#define SIZE_THREAD (64*1024)

/* Prototypes des variables globales et fonctions locales */
int thread_construct(struct thread *th, int is_main, int adding_type);
void run_thread(struct thread * next_running_thread);
void * th_intermediare(void * res);
static void thread_init() __attribute__ ((constructor));
static void thread_quit() __attribute__ ((destructor));

static struct thread_list ready_list;
static struct thread_list waiting_list;
static struct thread_list sleeping_list;
static ucontext_t exiting_context;

static struct thread *running;

/* Définitions des fonctions locales */
void thread_destruct(struct thread * th){
  if(running != th)
    list_del(&th->node);
  if(!th->is_main){
    free(th->uc.uc_stack.ss_sp);
  }
}

int thread_construct(struct thread *th, int is_main, int adding_type){
  th->status = READY;
  th->signal = 0;
  th->is_main = is_main;
  th->adding_type = adding_type;
  th->is_cancelable = PTHREAD_CANCEL_ENABLE;
  th->to_cancel = 0;
  th->parent = NULL;
  ucontext_t * uc = &th->uc;
  int i;
  for(i = 0; i<NB_SIGNALS; i++)
    th->tab_signal[i]=NULL;
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

void run_thread(struct thread * next_running_thread){
  thread_preemption_disable();
  if(next_running_thread == NULL)
  {
    running = NULL;
    setcontext(&exiting_context);
  }
  running = next_running_thread;
  setcontext(&next_running_thread->uc);
  thread_preemption_enable();
}

void * th_intermediaire(void *(*func)(void *), void *funcarg){
  thread_preemption_enable();
  void * res = func(funcarg);
  thread_exit(res);
  assert(0); //on n'est pas censé arriver là
  return NULL;
}

static void thread_init(){
  list_head_init(&ready_list.children);
  ready_list.num_children = 0;
  list_head_init(&waiting_list.children);
  waiting_list.num_children = 0;
  list_head_init(&sleeping_list.children);
  sleeping_list.num_children = 0;

  // initialising main_thread
  running = (struct thread *)malloc(sizeof(struct thread));
  thread_construct(running, 1, 0);

  thread_preemption_init();
  thread_preemption_enable();
}

static void thread_quit(){
  thread_preemption_disable();
  if(running){
    thread_destruct(running);
    free(running);
  }
  struct thread_list * list[] = {&waiting_list, &sleeping_list, &ready_list};
  struct thread * thread;
  int i;
  for(i = 0; i < 3; ++i){
    while(NULL != (thread = pop_from_list(list[i]))){
      thread_destruct(thread);
      free(thread);
    }
  }
  thread_preemption_quit();
}


#ifndef USE_PTHREAD

/* Fonctions de la bibliothèque */


extern struct thread * thread_self(void){
  return running;
}

extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
  // appel avec ajout en FIFO
  return thread_create_a(newthread, func, funcarg, 0);
}


extern int thread_create_a(thread_t *newthread, void *(*func)(void *), void *funcarg, int adding_type){
 
  thread_preemption_disable();
  ucontext_t * uc;
  //allocation newthread
  *newthread = (struct thread *) malloc(sizeof(struct thread));
  
  if(!newthread){
    perror("malloc newthread dans thread_create");
    return -1;
  }
  
  // initialisation newthread
  uc = &(*newthread)->uc;
  if(thread_construct(*newthread, 0, adding_type)==-1){
    free(newthread);
    return -1;
  }
  
  makecontext(uc, (void (*)(void)) th_intermediaire, 2, func, funcarg);
  (*newthread)->retval = NULL;
  add_in_list(&ready_list, *newthread);
  thread_preemption_enable();
  thread_yield();
  return 0;
}



extern int thread_yield(void){
  thread_preemption_disable();
  //place le thread courant dans la liste des threads ready
  struct thread * top = chose_next_running_thread(&ready_list);
  while (top != NULL && top->to_cancel && top->is_cancelable == PTHREAD_CANCEL_ENABLE){
    top->status = CANCELED;
    top->retval = PTHREAD_CANCELED;
    list_del(&top->node);
    add_in_list(&waiting_list,top);
    top = chose_next_running_thread(&ready_list);
  }
  if (top == NULL)
    return 0;
  add_in_list(&ready_list, running);
  struct thread * prev = running;
  running = top;
  int sig = get_signal(running->signal);
  if(running->tab_signal[sig])
    running->tab_signal[sig](sig);
  signal_done(&running->signal);
  int err = swapcontext(&prev->uc, &top->uc);
  thread_preemption_enable();
  return err;
}


extern int thread_join(thread_t thread, void **retval){
  if (thread->parent != NULL){
    perror("thread déjà attendu par qqn d'autre");
    return 1;
  } else if (thread == running){
    perror("impossible de se joindre soi-même");
    return 1;
    // section suivante destinée à savoir si un thread existe ou non
    // trop lente à l'exécution : parcours de toutes les listes
    /*
  } else if ((exist_thread(&ready_list, thread) == 0) && (exist_thread(&waiting_list, thread) == 0) && (exist_thread(&sleeping_list, thread) == 0)){
    perror("thread non existant");
    return 1;
    */
  } else  
    thread->parent = running;
  thread_preemption_disable();
  if (thread->to_cancel && thread->is_cancelable == PTHREAD_CANCEL_ENABLE) {
    list_del(&thread->node);
    thread->status = CANCELED;
    thread->retval = PTHREAD_CANCELED;
  } else if (thread->status == READY) {
    add_in_list(&sleeping_list,running);
    struct thread * top = chose_next_running_thread(&ready_list);
    struct thread * prev = running;
    running = top;
    swapcontext(&prev->uc, &top->uc);
  }
  assert (thread->status != READY);
  if (retval)
    *retval = thread->retval;
  thread_destruct(thread);
  free(thread);
  thread_preemption_enable();
  return 0;
}


extern void thread_exit(void *retval){
  thread_preemption_disable();
  running->retval = retval;
  running->status = WAITING;
  add_in_list(&waiting_list, running);

  if (running->is_main){
    struct thread * valid_thread;
    if (running->parent != NULL){
      valid_thread = running->parent;
      list_del(&valid_thread->node);
    }
    else 
      valid_thread = chose_next_running_thread(&ready_list);
    running = valid_thread;
    getcontext(&exiting_context);
    swapcontext(&exiting_context, &valid_thread->uc);
    exit(0);
  } else {
    if (running->parent != NULL){
      list_del(&running->parent->node);
      run_thread(running->parent);
    } else {
      run_thread(chose_next_running_thread(&ready_list));
    }
  }
  thread_preemption_enable();
}


/* autorise l'annulation ou non d'un thread, par défaut acceptée 
 * les états acceptés sont PTHREAD_CANCEL_ENABLE
 * et PTHREAD_CANCEL_ENABLE
 * retourne erreur sinon
 */
extern int thread_setcancelstate(int state, int *oldstate){
  if (oldstate)
    *oldstate = running->is_cancelable;
  if (state == PTHREAD_CANCEL_ENABLE || state == PTHREAD_CANCEL_DISABLE){
    running->is_cancelable = state;
    return 0;
  }
  perror("état d'annulation invalide");
  return -1;
}

/* annule le thread en cours au prochain yield sur celui-ci */

extern int thread_cancel(thread_t thread){
  if (thread == running) {
    perror("cela a-t-il un sens de s'annuler soi-même ?");
    return -1;
  }
  thread->to_cancel = 1;
  return 0;
}

int thread_kill(thread_t thread,int sig){
  if (thread->tab_signal[sig])
    add_signal(&thread->signal, sig);
  return 0;
}

extern void (*thread_signal(int signum,void (*new_sa_handler)(int)))(int){
  struct thread *thread = thread_self();
  thread->tab_signal[signum] = new_sa_handler;
  return new_sa_handler;
}




#endif
