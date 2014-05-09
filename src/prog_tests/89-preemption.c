#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "thread.h"

long long glob = 0;

void * loop(void * unused)
{
  for(;;)
    ++glob;
}

int main(int argc, char * argv[])
{
  int i = 0;
  thread_t thread;
  thread_create(&thread, loop, NULL);
  for(i = 0; i < 10; ++i)
  {
    printf("main running, infinite loop steps ran : %ld\n", glob);
    thread_yield();
  }
  exit(0);
}
