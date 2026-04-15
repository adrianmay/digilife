#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include "sleep.h"

#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

uint8_t  wrapSub8U  (uint8_t  a, uint8_t  b) { return a - b; }
uint32_t wrapSub32U (uint32_t a, uint32_t b) { return a - b; }
uint64_t wrapSub64U (uint64_t a, uint64_t b) { return a - b; }
uint8_t  wrapAdd8  (uint8_t  a,  uint8_t b) { return a + b; }
uint32_t wrapAdd32 (uint32_t a, uint32_t b) { return a + b; }
uint64_t wrapAdd64 (uint64_t a, uint64_t b) { return a + b; }
int8_t   wrapSub8S (uint8_t  a, uint8_t  b) { return a - b; }

Nanosecs age(clockid_t what) {
  struct timespec ts;
  clock_gettime(what, &ts); 
  return ts.tv_sec*1000000000 + ts.tv_nsec;
}

Nanosecs ageOfTime()    { return age(CLOCK_REALTIME); }
Nanosecs ageOfProcess() { return age(CLOCK_PROCESS_CPUTIME_ID); }
Nanosecs ageOfThread()  { return age(CLOCK_THREAD_CPUTIME_ID); }

void sleepS_(int s) {
  struct timespec ts;
  ts.tv_sec=s;
  ts.tv_nsec=0;
  clock_nanosleep(CLOCK_PROCESS_CPUTIME_ID, 0, &ts, 0);
}

void sleepNs_(Nanosecs ns) {
  struct timespec ts;
  ts.tv_sec =ns/1000000000;
  ts.tv_nsec=ns%1000000000;
  clock_nanosleep(CLOCK_PROCESS_CPUTIME_ID, 0, &ts, 0);
}

// Launch a thread that sleeps for a while and can be killed.
// Join with that thread.
// This can be started before doing work in the main thread, 
//   and if a KILL turns up during that work the join will
//   return immediately.

void * sleeperS(void * p) {
  int s = (int) p;
  sleepS_(s);
  return 0;
}

void * sleeperNs(void * p) {
  Nanosecs ns = (Nanosecs) p;
  sleepNs_(ns);
  return 0;
}

pthread_t sleepS(int s) {
  pthread_t tid;                                             
  if (pthread_create(&tid, 0, sleeperS, (void*)s)) return tid;
  else return 0;
}

pthread_t sleepNs(Nanosecs ns) {
  pthread_t tid;                                             
  pthread_create(&tid, 0, sleeperNs, (void*)ns);
  return tid;
}

void wait(pthread_t tid) { pthread_join(tid, 0); }
void wake(pthread_t tid) { pthread_cancel(tid); } _create

