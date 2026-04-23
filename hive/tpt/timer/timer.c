#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "h.h"
#define TIMER_SIG (SIGRTMIN)

static pthread_mutex_t bomb_mutex = PTHREAD_MUTEX_INITIALIZER;
static timer_t timer;
static sigset_t set;
static struct sigevent sev;

static pid_t gettid_linux(void) { return (pid_t)syscall(SYS_gettid); }

static void lock(bool lock) {
  if (lock) pthread_mutex_lock  (&bomb_mutex);
  else      pthread_mutex_unlock(&bomb_mutex);
}

static void arm(Nanosecs nsRel) {
  struct itimerspec its;
  memset(&its, 0, sizeof(its));
  if (nsRel) {
    lldiv_t qr = lldiv(nsRel, 1000000000);
    its.it_value.tv_sec = qr.quot;
    its.it_value.tv_nsec= qr.rem;
  } else its.it_value.tv_sec = its.it_value.tv_nsec = 0;
  timer_settime(timer, 0, &its, 0);
}

static void wait() {
  lock(false);
  siginfo_t info;
  while(TIMER_SIG != sigwaitinfo(&set, &info)) ;
  lock(true);
}

void initXXTimer() {
  sigemptyset(&set);
  sigaddset(&set, TIMER_SIG);
  pthread_sigmask(SIG_UNBLOCK, &set, 0); 
  memset(&sev, 0, sizeof(sev));
  sev.sigev_notify = SIGEV_THREAD_ID;
  sev.sigev_signo  = TIMER_SIG;
  sev._sigev_un._tid = gettid_linux();
  if (timer_create(CLOCK_PROCESS_CPUTIME_ID, &sev, &timer) != 0) {
      perror("timer_create");
      exit(1);
  }
}

void workOnXXTimer(Worker worker) {
  Nanosecs nsRel;
  lock(true);
  bool changeTimer = worker(&nsRel);
  if (changeTimer) arm(nsRel);
  lock(false);
}

void loopOnXXTimer(Worker worker) {
  Nanosecs nsRel;
  while (1) {
    lock(true);
    bool cont = worker(&nsRel);
    if (!cont) { lock(false); return; }
    arm(nsRel); 
    wait();
  }
}

