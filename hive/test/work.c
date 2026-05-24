#include <stdio.h>
#include <unistd.h>
#include "types.h"
#include "test.h"
#include "core/h.h"
#include "misc/h.h"
#include "globals/h.h"
#include "work/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/h.h"


typedef struct {
  char name[100];
  B setup;
  B expect;
  V cleanup;
  Tocks duration;
} Test;

bool noMoreRent = false;

#define DEFAULT_MOB_CASH 2000
#define DEFAULT_MSG_CASH 1000
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

////////////////////////////////
bool init0() {
  makeOneMob();
  return true;
}

bool expect0(void) { return true; }
void cleanup0(void) {}

bool init1() {
  MobIx i = makeOneMob();
  makeOneMsg(i);
  return true;
}
Test ts[] = {
  {"Test 0", init0, expect0, cleanup0, 2000},
};

#define NUM_TESTS (sizeof(ts)/sizeof(Test))

void onMobsExtinct(void) {
  noMoreRent = true;
}

bool expectWhenExtinct(BV expect, Tocks dur) { 
  updateTocks();
  Tocks start = tocksNow();
  printf("start=%d\n", start);
  //thisWorkerThread(0);
  while (!noMoreRent) ;
  updateTocks();
  Tocks end = tocksNow();
  printf("end=%d\n", end);
  printf("thisWorkerThread exited\n");
  TockDiff dr = end - start;
  assertDoubleApprox((double)dr, (double)dur, 0.02);
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


void * rentCollector(void * p) {
  (void)p;
  while (!noMoreRent) {
    //printf(".\n");
    hotelOfMobs.kill();
    usleep(1000);
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

