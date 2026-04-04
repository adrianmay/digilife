
#include <pthread.h>
void x() {
  printf("%d\n", sizeof(pthread_rwlock_t));
}

void * interrupter(void * p) {
  pthread_t * pTid = (pthread_t*) p;
  sleepS_(1);
  wake(*pTid);
}

bool trysleep() {
  printf("Starting sleep test at %'lld\n", ageOfTime());
  pthread_t mtid, stid = sleepS(2);
  pthread_create(&mtid, 0, interrupter, &stid);
  wait(stid);
  printf("Leaving sleep test at %'lld\n", ageOfTime());
  return true;
}

///////////////////////////////////////////////////////

void * sweat(void *) {
  for (int b=0;b<10;b++) {
    uint64_t z=1;
    for (int a=0;a<99999999;a++)  
      z*=a;
  }
}

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

