#pragma GCC diagnostic ignored "-Wunused-function"
#include "test.h"
#include "misc/h.h"
#include "MobBulk_pile/1.h"
#include "Mob_timer/h.h"

// Mobs:
//   No set -> wait til master timeout
//   Killer thread wait
//   Worker thread abort
//   Multiple ones

Nanosecs ns;
pthread_t sweat_pid;

void * unblockAndSweat(void * p) {
  (void)p;
  unblockMobTimerSignal();
  sweat_forever(0);
  return 0;
}

static bool init() {
  sweat_pid = background(unblockAndSweat); // Got to do work to advance CPU time ...
  initMobTimer(); 
  return true; 
}

static void cleanup() { 
  unitMobTimer();
  pthread_cancel(sweat_pid);
}

// static int workerWork(Nanosecs * pNsRel) { return 0; }

static int killerWorkQuit(Nanosecs * pNsRel) { return QUIT; }
static bool quits() {
  loopOnMobTimer(killerWorkQuit, 3000000000);
  return true;
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

bool pokeWorker(MobBulkIndex i, Nanosecs * pNs) { *pNs = 500000000; return SET; }
void * pokeLater(void * p) { (void)p;
  sleepNs(1000000000);
  workOnMobTimer(pokeWorker, (MobBulkIndex) {0});
  return 0;
}
static bool quitsSooner() {
  beenhere=false;
  pthread_t pid;
  pthread_create(&pid, 0, pokeLater, 0);
  TIME_VOID_PROC(loopOnMobTimer(looperQuitInABit, 3000000000));
  assertLongCond(ns, >1400000000ull)
  assertLongCond(ns, <1600000000ull)
  return true;
}

static bool test() { 
  return 
//    quits() &&
    quitsInABit() &&
//    quitsSooner() &&
    true;
}

bool timer() { return bkt("timer", init, test, cleanup); }

