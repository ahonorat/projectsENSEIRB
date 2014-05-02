int thread_kill(thread_t thread,int sig);

int thread_sigaction(int signum,const struct sigaction *act,struct sigaction *oldact);


