/* A simple signal-based preemption module. Incompatible with any program wishing to use the alarm. */
#include <time.h>
#include <stdio.h>

#include "thread.h"

#define TIMESLICE ((long long) 1e6)

static struct sigevent sev;
static struct itimerspec its;
static timer_t timerid;

int enablestatus = 0;


static void set_timer_preempt (long long time_slice)
{
  its.it_value.tv_sec = time_slice / 1000000000;
  its.it_value.tv_nsec = time_slice % 1000000000;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;
  
  timer_settime(timerid, 0, &its, NULL);
}

void preempt(int signum)
{
  if(signum == SIGALRM && enablestatus)
  {
    thread_yield();
  }
}

void thread_preemption_init()
{
  struct sigaction new;
  new.sa_handler = preempt;
  sigemptyset(&new.sa_mask);
  new.sa_flags = 0;
  sigaction(SIGALRM, &new, NULL);

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM;
  sev.sigev_value.sival_ptr = &timerid;
  timer_create(CLOCK_THREAD_CPUTIME_ID, &sev, &timerid);

  set_timer_preempt(TIMESLICE);
}

void thread_preemption_quit()
{
  timer_delete(timerid);
}

void thread_preemption_disable()
{
  enablestatus = 0;
}
void thread_preemption_enable()
{
  enablestatus = 1;
}
