#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread.h"

#define handle_error_en(en, msg)				\
  do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void *
thread_func(void *ignored_argument)
{
  int s;

  /* Disable cancellation for a while, so that we don't
     immediately react to a cancellation request */

  s = thread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  if (s != 0)
    handle_error_en(s, "pthread_setcancelstate");

  printf("thread_func(): started; cancellation disabled\n");
  sleep(2);
  thread_yield();

  printf("thread_func(): about to enable cancellation\n");

  s = thread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (s != 0)
    handle_error_en(s, "pthread_setcancelstate");

  /* sleep() is a cancellation point */

  sleep(2);        /* Should get canceled while we sleep */
  thread_yield();

  /* Should never get here */

  printf("thread_func(): not canceled!\n");
  return NULL;
}

int
main(void)
{
  thread_t thr;
  void *res;
  int s;
  /* Start a thread and then send it a cancellation request */

  s = thread_create(&thr, &thread_func, NULL);
  if (s != 0)
    handle_error_en(s, "pthread_create");

  sleep(1);           /* Give thread a chance to get started */

  printf("main(): sending cancellation request\n");
  s = thread_cancel(thr);
  if (s != 0)
    handle_error_en(s, "pthread_cancel");
  thread_yield();

  /* Join with thread to see what its exit status was */

  s = thread_join(thr, &res);
  if (s != 0)
    handle_error_en(s, "pthread_join");

  if (res == PTHREAD_CANCELED)
    printf("main(): thread was canceled\n");
  else
    printf("main(): thread wasn't canceled (shouldn't happen!)\n");
  exit(EXIT_SUCCESS);

}
