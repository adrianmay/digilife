/////////////////////////////////////////////
////// test/timer.c

#pragma GCC diagnostic ignored "-Wunused-function"
#include "test.h"
#include "misc/h.h"
#include "MobBulk_pile/1.h"
#include "Mob_timer/h.h"

Nanosecs ns;
pthread_t sweat_pid;

void * unblockAndSweat(void * p) {
  (void)p;
  unblockMobTimerSignal();
  sweat_forever(0);
  return 0;
}

static bool init() {
  initMobTimer(); 
  sweat_pid = background(unblockAndSweat); // Got to do work to advance CPU time ...
  return true; 
}

static void cleanup() { 
  unitMobTimer();
  pthread_cancel(sweat_pid);
}

static bool beenhere;
static int looperQuitInABit(Nanosecs * pNsRel) { 
  printf("looperQuitInABit at %'ld\n", ageOfProcess());
  if (beenhere) return QUIT; 
  *pNsRel = 2000000000; 
  beenhere = true; 
  printf("looperQuitInABit returning %'ld\n", *pNsRel);
  return SET|WAIT; 
}

static bool quitsInABit() {
  beenhere=false;
  TIME_VOID_PROC(loopOnMobTimer(looperQuitInABit, 10000000000));
  assertLongCond(ns, >1900000000ull)
  assertLongCond(ns, <2100000000ull)
  return true;
}

static bool test() { 
  return 
    quitsInABit() &&
    true;
}

bool timer() { return bkt("timer", init, test, cleanup); }

