/* A simple signal-based preemption module. Incompatible with any program wishing to use the alarm. */
#include <time.h>
#include <signal.h>
#include "thread.h"
#define TIMESLICE ((long long) 10000001)

static struct sigaction old;
struct itimerspec its;
timer_t timerid;
struct sigevent sev;


static void set_alarm (long long time_slice)
{
  its.it_value.tv_sec = time_slice / 1000000000;
  its.it_value.tv_nsec = time_slice % 1000000000;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;
  
  timer_settime(timerid, 0, &its, NULL);
}

void preempt(int signum)
{
  set_alarm(TIMESLICE);
  
  if(signum == SIGALRM)
  {
    thread_yield();
  }
}

void thread_preemption_init() __attribute__((constructor));
void thread_preemption_init()
{
  struct sigaction new;
  new.sa_handler = preempt;
  sigemptyset(&new.sa_mask);
  new.sa_flags = 0;
  sigaction(SIGALRM, &new, &old);

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM;
  sev.sigev_value.sival_ptr = &timerid;
  timer_create(CLOCK_THREAD_CPUTIME_ID, &sev, &timerid);

  set_alarm(TIMESLICE);
}


void thread_preemption_quit() __attribute__((destructor));
void thread_preemption_quit()
{
  sigaction(SIGALRM, &old, NULL);
}
