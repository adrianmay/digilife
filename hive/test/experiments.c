
pthread_t tid;

void * interrupter(void *) {
  sleepS(1);
  wake(tid);
}

void trysleep() {
  tid = initTiming();
  pthread_t pid;
  pthread_create(&pid, 0, interrupter, 0);
  sleepS(2);
  printf("leaving main thread\n");
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

