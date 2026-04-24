/////////////////////////////////////////////
////// lib/timer.c

#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "YY_pile/1.h"
#include "misc/h.h"
#include "h.h"
#define TIMER_SIG (SIGRTMIN+4)

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static timer_t timer;
static sigset_t set;
static struct sigevent sev;

static pid_t gettid_linux(void) { return (pid_t)syscall(SYS_gettid); }

void lockXXTimer(bool lock) {
  if (lock) pthread_mutex_lock  (&mutex);
  else      pthread_mutex_unlock(&mutex);
}

static void lock(bool lock) { lockXXTimer(lock); }

static void arm(Nanosecs nsRel) {
  struct itimerspec its;
  nsToTs(nsRel, &its.it_value);
  printf("arming to %'lds,  %'ldns\n", its.it_value.tv_sec, its.it_value.tv_nsec);
  timer_settime(timer, 0, &its, 0);
}

static void wait() {
  siginfo_t info;
  lock(false);
  while(TIMER_SIG != sigwaitinfo(&set, &info)) ;
  lock(true);
}

static void waitMax(Nanosecs max) {
  struct timespec ts;
  nsToTs(max, &ts);
  siginfo_t info;
  lock(false);
  int res;
  do {
    printf("waitMax before sigtimedwait at %'ld\n", ageOfProcess());
    res = sigtimedwait(&set, &info, &ts);
    sigset_t pending;
    sigpending(&pending);
    if (sigismember(&pending, TIMER_SIG)) {
        printf("Signal is pending but not caught\n");
    }
    printf("waitMax after sigtimedwait at %'ld\n", ageOfProcess());
  } 
  while (!( res == TIMER_SIG || (res==-1 && errno==EAGAIN)));
  lock(true);
}

void blockXXTimerSignal() { 
  sigemptyset(&set);
  sigaddset(&set, TIMER_SIG);
  pthread_sigmask(SIG_BLOCK, &set, 0); 
}

void unblockXXTimerSignal() { 
  sigemptyset(&set);
  sigaddset(&set, TIMER_SIG);
  pthread_sigmask(SIG_UNBLOCK, &set, 0); 
}

void initXXTimer() {
  blockXXTimerSignal();
  memset(&sev, 0, sizeof(sev));
  //sev.sigev_notify = SIGEV_THREAD_ID;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo  = TIMER_SIG;
  sev._sigev_un._tid = gettid_linux();
  if (timer_create(CLOCK_PROCESS_CPUTIME_ID, &sev, &timer) != 0) {
      perror("timer_create");
      exit(1);
  } else printf("Timer created\n");
}

void unitXXTimer() {  
  timer_delete(timer);
}

void workOnXXTimer(Worker worker, YYIndex iYY) {
  Nanosecs nsRel;
  lock(true);
  bool set = worker(iYY, &nsRel);
  if (set) arm(nsRel);
  lock(false);
}


void loopOnXXTimer(Looper looper, Nanosecs max) {
  Nanosecs nsRel;
  printf("loopOnXXTmer before lock at %'ld\n", ageOfProcess());
  lock(true);
  printf("loopOnXXTmer after lock at %'ld\n", ageOfProcess());
  while (1) {
    int flags = looper(&nsRel);
    if (flags & QUIT) { lock(false); return; }
    if (flags & SET) arm(nsRel); 
    if (flags & WAIT) {
      printf("loopOnXXTmer before wait at %'ld\n", ageOfProcess());
      if (max) waitMax(max);
      else wait();
      printf("loopOnXXTmer after wait at %'ld\n", ageOfProcess());
    }
  }
}

