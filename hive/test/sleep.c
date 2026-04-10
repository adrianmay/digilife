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

bool trysleep() {
  printf("Starting sleep test at %'lld\n", ageOfTime());
  pthread_t mtid, stid = sleepS(2); // stid will sleep for 2s process cpu time
  pthread_create(&mtid, 0, interrupter, &stid); // mtic will kill it after 1s
  background(sweat_forever); // Got to do work to advance CPU time ...
  background(sweat_forever); // ... at 2s per second.
  wait(stid); // Wait til original timer exits
  printf("Leaving sleep test at %'lld\n", ageOfTime()); // Expect it to have taken 0.5s of CPU process time
  return true;
}

bool sleep() { return trysleep(); }

