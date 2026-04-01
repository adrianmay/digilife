#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include "time.h"

uint8_t  wrapSubtract8 (uint8_t  a, uint8_t  b) { return a - b; }
uint32_t wrapSubtract32(uint32_t a, uint32_t b) { return a - b; }
uint64_t wrapSubtract64(uint64_t a, uint64_t b) { return a - b; }
uint8_t  wrapAdd8      (uint8_t  a,  uint8_t b) { return a + b; }
uint32_t wrapAdd32     (uint32_t a, uint32_t b) { return a + b; }
uint64_t wrapAdd64     (uint64_t a, uint64_t b) { return a + b; }

Nanosecs age(clockid_t what) {
  struct timespec ts;
  clock_gettime(what, &ts); 
  return ts.tv_sec*1000000000 + ts.tv_nsec;
}

Nanosecs ageOfProcess() { return age(CLOCK_PROCESS_CPUTIME_ID); }
Nanosecs ageOfThread()  { return age(CLOCK_THREAD_CPUTIME_ID); }

void signal_handler(int sig) { }
void defangSignal() {
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGALRM, &sa, NULL);
}

// First do this in the thread that will sleep and be interrupted,
//   keeping the returned thread id where the interrupting thread
//   can see it:
pthread_t initTiming() {
  defangSignal();
  return pthread_self();
}

// Then sleep in that thread:
void sleepS(int s) {
  struct timespec ts;
  ts.tv_sec=s;
  ts.tv_nsec=0;
  nanosleep(&ts, 0);
}

void sleepNs(Nanosecs ns) {
  struct timespec ts;
  ts.tv_sec=ns/1000000000;
  ts.tv_nsec=ns%1000000000;
  nanosleep(&ts, 0);
}

// Interrupt it from a different thread with this:
void wake(pthread_t tid) { pthread_kill(tid, SIGALRM); }


