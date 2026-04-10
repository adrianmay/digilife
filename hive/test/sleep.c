#include "test.h"
#include "sleep.h"

int processAge() {
  printf("PROCESS AGE TEST\n");
  printf("Age before 1s sleep    : %'lld\n", ageOfProcess());
  sleepS(1);
  printf("Age after  1s sleep    : %'lld\n", ageOfProcess());
  sweat(0);
  printf("Age after sweat        : %'lld\n", ageOfProcess());
  pthread_t pid;
  pthread_create(&pid, 0, sweat, 0);
  pthread_create(&pid, 0, sweat, 0);
  pthread_create(&pid, 0, sweat, 0);
  sweat(0);
  printf("Age after 4 more sweats: %'lld\n", ageOfProcess());
  printf("\n");
}

void * interrupter(void * p) {
  pthread_t * pTid = (pthread_t*) p;
  sleepS_(1);
  wake(*pTid);
}

void * worker(void *) {
  while(true) sweat(0);
}

bool trysleep() {
  printf("Starting sleep test at %'lld\n", ageOfTime());
  pthread_t wtid1, wtid2, mtid, stid = sleepS(2); // stid will sleep for 2s process cpu time
  pthread_create(&mtid, 0, interrupter, &stid); // mtic will kill it after 1s
  pthread_create(&wtid1, 0, worker, 0); // Got to do work to advance CPU time ...
  pthread_create(&wtid2, 0, worker, 0); // ... at 2s per second.
  wait(stid); // Wait til original timer exits
  printf("Leaving sleep test at %'lld\n", ageOfTime()); // Expect it to have taken 0.5s of CPU process time
  return true;
}

bool sleep() { return trysleep(); }

