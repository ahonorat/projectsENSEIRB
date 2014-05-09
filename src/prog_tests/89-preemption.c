#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "thread.h"

void * loop(void * ignored)
{
  for(;;)
    ;
}

int main(int argc, char * argv[])
{
  int i = 0;
  thread_t thread;
  thread_create(&thread, loop, NULL);
  for(int i = 0; i < 10; ++i)
  {
    thread_yield();
    printf("main running\n");
  }
  exit(0);
}
