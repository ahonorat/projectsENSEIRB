#include "thread.h"



int main(int argc, char **argv){

  thread_t *mon_thread;

  int r = thread_create(mon_thread, void *(*func)(void *), void *funcarg);

  



}
