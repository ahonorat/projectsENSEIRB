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
  enum status_t status;
  void * retval;
#ifndef NDEBUG
  int valgrind_stackid;
#endif
};

#endif
