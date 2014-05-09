#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <ucontext.h>
#include <valgrind/valgrind.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"


enum status_t {
  READY,
  WAITING
};

enum signal_t {
  THREAD_SIGHUP=15, // controlling terminal has been closed.
  THREAD_SIGINT=6, // ctrl+C from controlling terminal.
  THREAD_SIGQUIT=2, // ctrl+\ with core dump
  THREAD_SIGILL=3, // illegal instruction
  THREAD_SIGABRT=4, // tell the process to abort
  THREAD_SIGFPE=5, // floating point exception (ex:divide by 0) 
  THREAD_SIGKILL=0, // terminate IMMEDIATELY a process #nocaught#noignored
  THREAD_SIGSEGV=7, // seg fault violation
  THREAD_SIGPIPE=8, // sent when trying to write to a pipe, without connected process
					// on the other end.
  THREAD_SIGALRM=9, // sent when time limit specified elapses.
  THREAD_SIGTERM=10, // nearly identical to SIGINT but without ctrl+c
  THREAD_SIGUSR1=17, // user defined conditions
  THREAD_SIGUSR2=18, // user defined conditions
  THREAD_SIGCHLD=13, // modification of a child process status (terminates, interrupted, resumes)
  THREAD_SIGCONT=14, // continue a pause process
  THREAD_SIGSTOP=1, // pauses the signal #nocaught#noignored
  THREAD_SIGTSTP=16, // ctrl + z : pausing from controlling terminal
  THREAD_SIGTTIN=11, // received when trying to readin the tty while in background #nevercalledwithoutterminal
  THREAD_SIGTTOU=12 // received when trying to write out in the tty while in background
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
