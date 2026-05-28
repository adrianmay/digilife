#pragma GCC diagnostic ignored "-Wunused-function"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "types.h"
#include "test.h"
#include "core/h.h"
#include "misc/h.h"
#include "globals/h.h"
#include "work/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/h.h"

typedef struct {
  char name[100]; B setup;
  int expect;
  V cleanup;
  Tocks duration;
} Test;

Timer workerTimer;

bool noMoreRent = false;

#define DEFAULT_MOB_CASH 2000
#define RICH_MSG_CASH 1200
#define POOR_MSG_CASH 700
#define DEFAULT_BID 0.1
// Approximately cycles:
#define DEFAULT_EFFORT 10000000
//Extinct

MobIx makeOneMobWithCashAndEffort(Cash cash, int effort) {
  Mob * pMob;
  MobIx i = hotelOfMobs.alloc(cash, &pMob, 0);
  pMob->body.effort = effort;
  return i;
}

bool makeMsgWithCashAndBid(MobIx i, Cash cash, CpuBid bid) {
  emit(cash, bid, i, i);
  return true;
}

MobIx makeOneMobWithEffort(int effort)   { return makeOneMobWithCashAndEffort(DEFAULT_MOB_CASH, effort); }
MobIx makeOneMob()                       { return makeOneMobWithEffort(DEFAULT_EFFORT); }
bool makeMsgWithCash(MobIx i, Cash cash) { return makeMsgWithCashAndBid(i, cash, DEFAULT_BID); }
bool makeMsgWithBid(MobIx i, CpuBid bid) { return makeMsgWithCashAndBid(i, POOR_MSG_CASH, bid); }

////////////////////////////////
bool setup0() {
  makeOneMob();
  return true;
}

void cleanup0(void) {}

bool setup1() {
  MobIx i = makeOneMob();
  makeMsgWithCash(i, RICH_MSG_CASH);
  return true;
}

bool setup2() {
  MobIx i = makeOneMob();
  makeMsgWithCash(i, POOR_MSG_CASH);
  return true;
}

static bool setup3() {
  MobIx i = makeOneMobWithEffort(4000000);
  makeMsgWithCash(i, POOR_MSG_CASH);
  printf("Made poor message\n");
  return true;
}

static bool setup4() {
  MobIx i1 = makeOneMobWithCashAndEffort(5000, DEFAULT_EFFORT+1);
  MobIx i2 = makeOneMobWithCashAndEffort(5000, DEFAULT_EFFORT+2);
  makeMsgWithCashAndBid(i1, 2000, 0.1);
  makeMsgWithCashAndBid(i2, 2000, 0.01);
  //hotelOfMobs.show();
  //raffleOfMsgs.show();
  printf("Made two rich messages\n");
  return true;
}


static bool setup5() {
  MobIx i1 = makeOneMobWithCashAndEffort(5000, DEFAULT_EFFORT+1);
  MobIx i2 = makeOneMobWithCashAndEffort(3000, DEFAULT_EFFORT+2);
  makeMsgWithCashAndBid(i1, 2000, 0.20);
  makeMsgWithCashAndBid(i2, 10, 0.001);
  printf("Made one slow and one very poor messages\n");
  return true;
}

void onMobsExtinct(void) {
//  printf("onMobsExtinct\n");
  noMoreRent = true;
  raffleOfMsgs.quit();
}

bool expectWhenExtinct(int expect, Tocks dur) { 
  updateTocks();
  Worker * pW = thisWorker(0);
  pW->output = 0; pW->firstStarted = pW->lastStarted = pW->lastEnded = 0;
  runWorker(pW);
  updateTocks();
  assertInt(pW->output, expect);
  Cycles dr = pW->lastEnded - pW->firstStarted;
  printf("dr=%'ld\n", dr);
  assertDoubleApprox((double)dr, (double)dur, 0.1);
  return true;
}

bool testCase(Test * t) {
  printf("\nTESTING: %s\n", t->name);
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
    hotelOfMobs.kill();
    usleep(1000);
  }
  return 0;
}

Test ts[] = {
  {"One mob, no msg, starves. No thread cpu usage", setup0, 0, cleanup0, 0},
  {"Add a rich msg, mob dies much sooner, message runs to completion", setup1, 2, cleanup0, 10000000},
  {"Add a poor msg, mob dies a bit sooner, message preempted", setup2, 1, cleanup0, 7000000},
  {"Economical mob can afford it", setup3, 2, cleanup0, 3700000},
  {"Choice depends on bid", setup4, 22, cleanup0, 20000000}, //Sometimes get 12
  {"Message can die of memory rent before being called", setup5, 11, cleanup0, 9000000}, //Sometimes get 12
};

#define NUM_TESTS (sizeof(ts)/sizeof(Test))

bool work() {
  bool ret = true;
  background(sweat_forever); // Got to do work to advance CPU time ...
  openGlobals();
  hotelOfMobs.open();
  raffleOfMsgs.open();
  pthread_t rentColPid;

  for (int t=0;t<NUM_TESTS;t++)  {
    noMoreRent = false;
    pthread_create(&rentColPid, 0, rentCollector, 0);
    if (!testCase(ts+t)) {
      ret = false;
      break;
    }
    noMoreRent=true;
    pthread_join(rentColPid, 0);
  }
  printf("\n");
  //waitWorkersAllDone(); 
  raffleOfMsgs.close(DELETE);
  hotelOfMobs.close(DELETE);
  closeGlobals(DELETE);
  return ret;
}

