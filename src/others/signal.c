int thread_kill(thread_t thread,int sig){
  thread->signal = sig;
  return 1;
}

int thread_sigaction(int signum,const struct sigaction *act,struct sigaction *oldact){
  if(act)
    act->handler(signum); 
  thread->signal = 0;

  return 1;

}

