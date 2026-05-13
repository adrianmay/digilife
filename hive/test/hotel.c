#include "types.h"
#include <unistd.h>
#include "test.h"
#include "misc/h.h"
#include "perf/h.h"
#include "globals/h.h"
#include "Thing_hotel/Bulk.h"
#include "Thing_hotel/h.h"

static bool extinct;
void onThingsExtinct(void) { extinct = true; } 

// void onMobsExtinct() {}
// void onMsgsExtinct() {}

Cycles cycles;
ThingBulk * pThing;
ThingBulkIndex iThing;
static ThingBulkIndex iDonor = {0};

static bool init(void) {
  openGlobals();
  hotelOfThings.open(1000000);
  hotelOfThings.show();
  return true;
}

void killTilExtinct(void) {
  while (true) {
    updateTocks();
    //printf("killTilExtinct: tocks=%d, processCycles=%'ld\n", tocksNow(), readProcessCycles());
    hotelOfThings.kill(); 
    if (extinct) return;
    usleep(10000);
  }
}

bool testNoPop(void) {
  extinct = false;
  TIME_VOID_PROC(killTilExtinct()); 
  printf("testNoPop: %'ld\n", cycles);
  assertLongCond(cycles, <40000);
  return true;
}

void make(Index name, Cash cash) {
  bool recycledSlot;
  iThing = hotelOfThings.alloc(cash, iDonor, &pThing, &recycledSlot);
  pThing->body.name = name;
}

bool test1(void) {
  extinct = false;
  make(3, 2000);
  printf("Made in test1\n");
  TIME_VOID_PROC(killTilExtinct());
  printf("test1: %'ld\n", cycles);
  assertLongCond(cycles, <2100000000ull)
  assertLongCond(cycles, >1900000000ull)
  return true;
}

void * earn(void *) {
  sleepNs(1000000000);
  hotelOfThings.transfer(2000, iDonor, iThing);
  return 0;
}

bool testEarn(void) {
  extinct = false;
  printf("testEarn\n");
  hotelOfThings.show();
  make(4, 3000);
  hotelOfThings.show();
  background(earn);
  TIME_VOID_PROC(killTilExtinct());
  printf("testEarn: %'ld\n", cycles);
  assertLongCond(cycles, <5100000000ull)
  assertLongCond(cycles, >4900000000ull)
  return true;
}

void * rob(void *) {
  sleepNs(1000000000);
  hotelOfThings.rob(iThing);
  return 0;
}

bool testRob(void) {
  extinct = false;
  printf("testRob\n");
  make(5, 9000);
  background(rob);
  TIME_VOID_PROC(killTilExtinct());
  printf("testRob: %'ld\n", cycles);
  assertLongCond(cycles, <1100000000ull)
  assertLongCond(cycles, > 900000000ull)
  return true;
}


void cleanupHotel(void) { hotelOfThings.close(DELETE); closeGlobals(DELETE); }

bool testHotel(void) {
  background(sweat_forever); // Got to do work to advance CPU time ...
  return 
    //testNoPop() &&
    test1() &&
    // testEarn() &&
    // testRob() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }


