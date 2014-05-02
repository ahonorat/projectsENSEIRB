#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "list.h"
#include <valgrind/valgrind.h>
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


enum status_t {
  READY,
  WAITING
};

enum signal_t {
  THREAD_SIGHUP,
  THREAD_SIGINT,
  THREAD_SIGQUIT,
  THREAD_SIGILL,
  THREAD_SIGABRT,
  THREAD_SIGFPE,
  THREAD_SIGKILL,
  THREAD_SIGSEGV,
  THREAD_SIGPIPE,
  THREAD_SIGALRM,
  THREAD_SIGTERM,
  THREAD_SIGUSR1,
  THREAD_SIGUSR2,
  THREAD_SIGCHLD,
  THREAD_SIGCONT,
  THREAD_SIGSTOP,
  THREAD_SIGTSTP,
  THREAD_SIGTTIN,
  THREAD_SIGTTOU
};



struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};

struct thread{
  int is_main;
  struct thread * parent;
  struct list_node node;
  ucontext_t uc;
  enum signal_t signal;
  enum status_t status;
  void * retval;
#ifndef NDEBUG
  int valgrind_stackid;
#endif
};

#endif
