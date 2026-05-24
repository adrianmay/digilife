#include <stdio.h>
#include <unistd.h>
#include "types.h"
#include "test.h"
#include "core/h.h"
#include "work/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/h.h"


typedef struct {
  char name[100];
  B setup;
  B expect;
  V cleanup;
  Cycles duration;
} Test;

#define DEFAULT_MOB_CASH 5000
#define DEFAULT_MSG_CASH 4500
#define DEFAULT_BID 10
//Extinct
MobIx makeOneMob() {
  MobIx i = hotelOfMobs.alloc(DEFAULT_MOB_CASH, 0, 0);
  return i;
}

bool makeOneMsg(MobIx i) {
  emit(DEFAULT_MSG_CASH, DEFAULT_BID, i, i);
  return true;
}

bool init0() {
  MobIx i = makeOneMob();
  makeOneMsg(i);
  return true;
}
void cleanup0(void) {}

bool expect0(void) { return true; }

Test ts[] = {
  {"Test 0", init0, expect0, cleanup0, 0},
};

#define NUM_TESTS (sizeof(ts)/sizeof(Test))

bool expectWhenExtinct(BV expect, Cycles dur) { 
  thisWorkerThread(0);
  printf("thisWorkerThread exited\n");
  return (*expect)();
}

bool testCase(Test * t) {
  printf("Testing %s\n", t->name);
  bool suc;
  if ( (suc=(*t->setup)()) ) {
    suc = expectWhenExtinct(t->expect, t->duration);
    (*t->cleanup)();
  }
  return suc;
}

bool noMoreRent = false;
void * rentCollector(void * p) {
  (void)p;
  while (!noMoreRent) {
    printf(".\n");
    hotelOfMobs.kill();
    usleep(100000);
  }
  return 0;
}

bool work() {
  bool ret = true;
  background(sweat_forever); // Got to do work to advance CPU time ...
  openGlobals();
  hotelOfMobs.open();
  raffleOfMsgs.open();

  pthread_t rentColPid;
  pthread_create(&rentColPid, 0, rentCollector, 0);

  for (int t=0;t<NUM_TESTS;t++)  {
    if (!testCase(ts+t)) {
      ret = false;
      break;
    }
  }
  noMoreRent = true;
  pthread_join(rentColPid, 0);
  printf("Renter done\n");
  waitWorkersAllDone(); 
  raffleOfMsgs.close(DELETE);
  hotelOfMobs.close(DELETE);
  closeGlobals(DELETE);
  return ret;
}

