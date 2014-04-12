#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "list.h"
#include <valgrind/valgrind.h>
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
#ifndef NDEBUG
  int valgrind_stackid;
#endif
};

struct thread_container
{
  struct list_node node;
  struct thread *thread;
};

#endif
