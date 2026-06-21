#include "types.h"
#include <unistd.h>
#include "test.h"
#include "misc/h.h"
#include "perf/h.h"
#include "globals/h.h"
#include "Thing_hotel/structs.h"
#include "Thing_pile/1.h"
#include "Thing_hotel/h.h"

// ALL OFF COS OF PERF STUFF 

#define NOTIFY_TOCKS 150

static bool extinct=true;
void onThingsExtinct(void) { extinct = true; }
bool onThingHotelGoDie(ThingIx i, Thing * pThing) { return true;}
void onThingRentCollected(Cash cash) { }
void onThingRentDefaulted(Cash cash) { }

Cycles cycles;
Thing * pThing;
ThingTact tThing;
ThingTact tGod;

static void tock() {
  hotelOfThings.raid();
}

static bool init(void) {
  onTestTock = tock;
  openGlobals();
  hotelOfThings.open();
  hotelOfThings.checkHotel(0);
  tGod = hotelOfThings.admit(0,0,0,0); //God
  return true;
}

Tocks killTilExtinct(void) {
  Tocks started = tocksNow();
  while (true) {
//    printf("killTilExtinct: tocks=%d, processCycles=%'ld\n", tocksNow(), readProcessCycles());
    //hotelOfThings.forAll(hotelOfThings.review);
    //hotelOfThings.kill();
    if (extinct) break;
    //hotelOfThings.show();
    notifyCycles(NOTIFY_TOCKS*GUESS_CYCLES_PER_TOCK);
    sleepNs(1000000);
  }
  Tocks ended = tocksNow();
  return ended - started;
}

bool testNoPop(void) {
  Tocks dur = killTilExtinct();
  printf("testNoPop: %d\n", dur);
  assertCond(dur, ==0);
  return true;
}

ThingTact make(Ix name, Cash cash) {
  bool recycledSlot;
  void stuff(ThingBody * p) { p->name = name; }
  tThing = hotelOfThings.admit(cash, stuff, &pThing, &recycledSlot);
  extinct=false;
  return tThing;
}

bool expectExtinctSoon(Cash cash) {
  Tocks dur = killTilExtinct();
  Tocks expect;
  //printf("expectExtinctSoon: dur=%d\n", dur);
  if (cash==0) 
    expect = 0;
  else {
    Tocks expectIdeal = cash / ( billableThingSize * tockPrice() );
     expect = ((expectIdeal - 1) / NOTIFY_TOCKS + 1) * NOTIFY_TOCKS ;
  }
  assertInt(dur, expect);
  return true;
}

bool test1(void) {
  Cash cash = 4000;
  make(3, cash);
  printf("Made in test1\n");
  expectExtinctSoon(cash);
  return true;
}

void * earn(void *) {
  hotelOfThings.richer(tThing, 2000);
  return 0;
}

bool testEarn(void) {
  printf("testEarn\n");
  make(4, 3000);
  hotelOfThings.richer(tThing, 2000);
  //hotelOfThings.show();
  //earn(0);
  expectExtinctSoon(5000);
  return true;
}

bool testRob(void) {
  printf("testRob\n");
  make(5, 9000);
  hotelOfThings.poorer(tThing, 0, Rob);
  expectExtinctSoon(150);
  return true;
}

bool testGod(void) {
  printf("testGod\n");
  make(6, 1000);
  make(5, 0);
  expectExtinctSoon(1000);
  hotelOfThings.collectRent(tThing);
  bool ff(Thing * pThing) { assertLong(pThing->rent.cash, -1200l); return true;}
  void f(Thing * pThing) { ff(pThing); }
  hotelOfThings.with(tThing, f);
  return true;
}

bool testAfterFree(void) {
  printf("testAfterFree\n");
  ThingTact tThing = make(6, 1000);
  expectExtinctSoon(1000);
  void f(Thing * p) { (void)p; }
  Woth w = hotelOfThings.with(tThing, f);
  assertInt(w, Dead);
  return true;
}

bool testBusy(void) {
  printf("testBusy\n");
  ThingTact tThing = make(6, 10000);
  //printf("nick: %x\n", tThing.n);
  void f(Thing * p) { (void)p; }
  Woth wi;
  void g(Thing * p) { 
    wi = hotelOfThings.with(tThing, f);
  }
  Woth wo = hotelOfThings.with(tThing, g);
  assertInt(wo, Ok);
  assertInt(wi, Busy);
  return true;
}

bool testFreeWhenBusy(void) {
  printf("testFreeWhenBusy\n");
  ThingTact tThing = make(6, 1);
  //printf("nick: %x\n", tThing.n);
  bool sameIndex2;
  void g(Thing * p) { 
    notifyCycles(2*GUESS_CYCLES_PER_TOCK);
    //hotelOfThings.poorer(tThing, 0, Rob);
    //hotelOfThings.raid();
    ThingTact t2 = make(7,1000);
    sameIndex2 = t2.i.i == tThing.i.i;
  }
  Woth wo = hotelOfThings.with(tThing, g);
  ThingTact t3 = make(8,1000);
  bool sameIndex3 = t3.i.i == tThing.i.i;
  assertInt(wo, Ok);
  assertInt(sameIndex2, false);
  assertInt(sameIndex3, true);
  return true;
}
void cleanupHotel(void) { hotelOfThings.close(HIDE); closeGlobals(HIDE); }

bool testHotel(void) {
  printf("Tock price: %f\n", tockPrice());
  printf("Billable size: %ld\n", billableThingSize);
  return
  //  testNoPop() &&
  //  test1() &&
  //  testEarn() &&
  //  testRob() &&
  //  testGod() &&
    //testAfterFree() &&
    testBusy() &&
    testFreeWhenBusy() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }


