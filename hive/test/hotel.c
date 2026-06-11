#include "types.h"
#include <unistd.h>
#include "test.h"
#include "misc/h.h"
#include "perf/h.h"
#include "globals/h.h"
#include "Thing_hotel/structs.h"
#include "Thing_pile/1.h"
#include "Thing_hotel/h.h"
#include "bit/MobBody.h"

// ALL OFF COS OF PERF STUFF 

#define NOTIFY_TOCKS 150

static bool extinct;
void onMobsExtinct() {}
void onMsgsExtinct() {}
void onThingsExtinct(void) { extinct = true; }
void onThingKilled(ThingIx i) { pileOfThings.free(i);}
void onThingRentCollected(Cash cash) { }
void onThingRentDefaulted(Cash cash) { }
void showMobBody(MobIx i, MobBody * p) {
  printf("code=<binary>\n");
}


Cycles cycles;
Thing * pThing;
ThingIx iThing;
ThingIx iGod;

static bool init(void) {
  openGlobals();
  hotelOfThings.open();
  iGod = hotelOfThings.admit(0,0,0); //God
  return true;
}

Tocks killTilExtinct(void) {
  Tocks started = tocksNow();
  while (true) {
//    printf("killTilExtinct: tocks=%d, processCycles=%'ld\n", tocksNow(), readProcessCycles());
    hotelOfThings.forAll(hotelOfThings.review);
    hotelOfThings.kill();
    if (extinct) break;
    //hotelOfThings.show();
    notifyCycles(NOTIFY_TOCKS*GUESS_CYCLES_PER_TOCK);
  }
  Tocks ended = tocksNow();
  return ended - started;
}

bool testNoPop(void) {
  extinct = false;
  Tocks dur = killTilExtinct();
  printf("testNoPop: %d\n", dur);
  assertCond(dur, ==0);
  return true;
}

void make(Ix name, Cash cash) {
  bool recycledSlot;
  iThing = hotelOfThings.admit(cash, &pThing, &recycledSlot);
  pThing->body.name = name;
}

bool expectExtinctSoon(Cash cash) {
  Tocks dur = killTilExtinct();
  Tocks expect;
  //printf("expectExtinctSoon: dur=%d\n", dur);
  if (cash==0) 
    expect = 0;
  else {
    Tocks expectIdeal = cash / (hotelOfThings.billableSize*tockPrice());
     expect = ((expectIdeal - 1) / NOTIFY_TOCKS + 1) * NOTIFY_TOCKS ;
  }
  assertInt(dur, expect);
  return true;
}

bool test1(void) {
  Cash cash = 4000;
  extinct = false;
  make(3, cash);
  expectExtinctSoon(cash);
  printf("Made in test1\n");
  return true;
}

void * earn(void *) {
  hotelOfThings.enrich(iThing, 2000);
  return 0;
}

bool testEarn(void) {
  extinct = false;
  printf("testEarn\n");
  make(4, 3000);
  hotelOfThings.enrich(iThing, 2000);
  //hotelOfThings.show();
  //earn(0);
  expectExtinctSoon(5000);
  return true;
}

bool testRob(void) {
  extinct = false;
  printf("testRob\n");
  make(5, 9000);
  hotelOfThings.rob(iThing);
  expectExtinctSoon(0);
  return true;
}

bool testGod(void) {
  extinct = false;
  printf("testGod\n");
  make(6, 1000);
  make(5, 0);
  expectExtinctSoon(1000);
  Cash godsCash = hotelOfThings.get(iThing)->rent.cash;
  assertLong(godsCash, -1200l);
  return true;
}

void cleanupHotel(void) { hotelOfThings.close(HIDE); closeGlobals(HIDE); }

bool testHotel(void) {
  printf("Tock price: %f\n", tockPrice());
  printf("Billable size: %ld\n", hotelOfThings.billableSize);
  return
    testNoPop() &&
    test1() &&
    testEarn() &&
    testRob() &&
    testGod() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }


