#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#ifndef _GNU_SOURCE
# define _GNU_SOURCE /* for pthread_yield() */
#endif

#include <ucontext.h>
#include <valgrind/valgrind.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "list.h"

enum status_t {
  READY,
  WAITING,
  CANCELED
};

#define NB_SIGNALS 24

enum signal_t {
  THREAD_SIGHUP=SIGHUP, // controlling terminal has been closed.
  THREAD_SIGINT=SIGINT, // ctrl+C from controlling terminal.
  THREAD_SIGQUIT=SIGQUIT, // ctrl+\ with core dump
  THREAD_SIGILL=SIGILL, // illegal instruction
  THREAD_SIGABRT=SIGABRT, // tell the process to abort
  THREAD_SIGFPE=SIGFPE, // floating point exception (ex:divide by 0) 
  THREAD_SIGKILL=SIGKILL, // terminate IMMEDIATELY a process #nocaught#noignored   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  THREAD_SIGSEGV=SIGSEGV, // seg fault violation
  THREAD_SIGPIPE=SIGPIPE, // sent when trying to write to a pipe, without connected process
					// on the other end.
  THREAD_SIGALRM=SIGALRM, // sent when time limit specified elapses.
  THREAD_SIGTERM=SIGTERM, // nearly identical to SIGINT but without ctrl+c
  THREAD_SIGUSR1=SIGUSR1, // user defined conditions                 ------------------------------------------------------------
  THREAD_SIGUSR2=SIGUSR2, // user defined conditions                 ------------------------------------------------------------
  THREAD_SIGCHLD=SIGCHLD, // modification of a child process status (terminates, interrupted, resumes)
  THREAD_SIGCONT=SIGCONT, // continue a pause process
  THREAD_SIGSTOP=SIGSTOP, // pauses the signal #nocaught#noignored                    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  THREAD_SIGTSTP=SIGTSTP, // ctrl + z : pausing from controlling terminal
  THREAD_SIGTTIN=SIGTTIN, // received when trying to readin the tty while in background #nevercalledwithoutterminal
  THREAD_SIGTTOU=SIGTTOU // received when trying to write out in the tty while in background
};


struct thread_list
{
  struct list_head children;
  unsigned int num_children;
};

struct thread{
  int is_main;
  int is_cancelable;
  int to_cancel;
  int adding_type;
  struct thread * parent;
  struct list_node node;
  ucontext_t uc;
  enum signal_t signal;
  enum status_t status;
  void * retval;
  void (*tab_signal[NB_SIGNALS])(int);
#ifndef NDEBUG
  int valgrind_stackid;
#endif
};

#endif
