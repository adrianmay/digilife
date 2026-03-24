#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
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

pthread_t main_thread_id;

void signal_handler(int sig) { }

void defangSignal() {
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGALRM, &sa, NULL);
}

void wake() {
  printf("killing main thread\n");
  pthread_kill(main_thread_id, SIGALRM);
}

void initTiming() {
  defangSignal();
  main_thread_id = pthread_self();
}

void * interrupter(void *) {
  sleepS(1);
  wake();
}

void trysleep() {
  initTiming();
  pthread_t pid;
  pthread_create(&pid, 0, interrupter, 0);
  sleepS(2);
  printf("leaving main thread\n");
}


