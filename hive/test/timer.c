#include "test.h"
#include "misc/h.h"
#include "Test_timer/h.h"

// Tests:
//   No set -> wait til master timeout
//   Killer thread wait
//   Worker thread abort
//   Multiple ones

Nanosecs ns;
pthread_t sweat_pid;

static bool init() {
  initTestTimer(); 
  sweat_pid = background(sweat_forever); // Got to do work to advance CPU time ...
  return true; 
}

static void cleanup() { 
  pthread_cancel(sweat_pid);
  unitTestTimer();
}

// static int workerWork(Nanosecs * pNsRel) { return 0; }

static int killerWorkQuit(Nanosecs * pNsRel) { return QUIT; }
static bool quits() {
  loopOnTestTimer(killerWorkQuit, 3000000000);
  return true;
}

static bool beenhere;
static int killerWorkQuitInABit(Nanosecs * pNsRel) { 
  if (beenhere) return QUIT; 
  *pNsRel = 2000000000; 
  beenhere = true; 
  return WAIT|SET; 
}

static bool quitsInABit() {
  beenhere=false;
  TIME_VOID_PROC(loopOnTestTimer(killerWorkQuitInABit, 3000000000));
  assertLongCond(ns, >1900000000ull)
  assertLongCond(ns, <2100000000ull)
  return true;
}

int pokeWorker(Nanosecs * pNs) { *pNs = 500000000; return SET; }
void * pokeLater(void * p) { (void)p;
  sleepNs(1000000000);
  workOnTestTimer(pokeWorker);
  return 0;
}
static bool quitsSooner() {
  beenhere=false;
  pthread_t pid;
  pthread_create(&pid, 0, pokeLater, 0);
  TIME_VOID_PROC(loopOnTestTimer(killerWorkQuitInABit, 3000000000));
  assertLongCond(ns, >1400000000ull)
  assertLongCond(ns, <1600000000ull)
  return true;
}

static bool test() { 
  return 
    quits() &&
    quitsInABit() &&
    quitsSooner() &&
    true;
}

bool timer() { return bkt("timer", init, test, cleanup); }

