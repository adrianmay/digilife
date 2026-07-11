#include "h.h"
#include "misc/api.h"
#include "globals/api.h"
#include "Thing_hotel/ix.h"
#include "bit/Thing.h"
#include "Thing_hotel/api.h"

static bool extinct=true;
void onThingsExtinct(void) { extinct = true; }
void onThingRentCollected(Cash cash) { }
void onThingRentDefaulted(Cash cash) { }

void onThingHotelGoDie(ThingIx i, Thing * pThing) {
//  St want = Idle;
//  for (int a=0; a<10*MAX_THS; a++) {
//    if (ths[a].t.i.i == i.i) {
//      printf("GoDie %i\n", i.i);
//      atomic_compare_exchange_strong(&ths[a].st, &want, Free);
//    }
//  }
}

Cycles cycles;
Thing * pThing;
ThingTact tGod, tThing;

static void tock() {
  hotelOfThings_raid();
}

void showThing(Thing * p) {
  printf("name=%d,code=<binary>\n", p->name);
}

static bool init(void) {
  onTestTock = tock;
  openGlobals();
  hotelOfThings_open(); //hotelOfThings_checkHotel(0);
  tGod = hotelOfThings_admit(0,true, 0,0,0); //God 
  return true;
}
 
void cleanupHotel(void) { hotelOfThings_close(Hide); closeGlobals(Hide); }

#define NOTIFY_TOCKS 10

Tocks killTilExtinct(void) {
  Tocks started = tocksNow();
  while (true) {
//    printf("killTilExtinct: tocks=%d, processCycles=%'ld\n", tocksNow(), readProcessCycles());
    //hotelOfThings_forAll(hotelOfThings_review);
    //hotelOfThings_kill();
    //hotelOfThings_show();
    if (extinct) break;
    notifyCycles(NOTIFY_TOCKS*GUESS_CYCLES_PER_TOCK);
//    sleepNs(1000);
  }
  Tocks ended = tocksNow();
  return ended - started;
}

bool expectExtinctSoon(Cash cash, int line) {
  Tocks dur = killTilExtinct();
  Tocks expect;
  if (cash==0) 
    expect = NOTIFY_TOCKS;
  else {
    Tocks expectIdeal = cash / ( billableThingSize * tockPrice() );
    expect = NOTIFY_TOCKS + ((expectIdeal - 1) / NOTIFY_TOCKS + 1) * NOTIFY_TOCKS ;
    printf("expectIdeal=%d expect=%d\n", expectIdeal, expect);
  }
  assertIntAtLine(dur, expect, line);
  return true;
}

ThingTact make(Ix name, Cash cash) {
  bool recycledSlot;
  void stuff(Thing * p) { p->name = name; }
  tThing = hotelOfThings_admit(cash, !cash, stuff, &pThing, &recycledSlot);
  extinct=false;
  return tThing;
}

bool testNoPop(void) {
  Tocks dur = killTilExtinct();
  printf("testNoPop: %d\n", dur);
  assertCond(dur, ==0);
  return true;
}

bool test1(void) {
  Cash cash = 40'000'000;
  make(3, cash);
  printf("Made in test1\n");
  expectExtinctSoon(cash, __LINE__);
  return true;
}

bool testEarn(void) {
  printf("testEarn\n");
  make(4, 30'000'000);
  Cash cash;
  hotelOfThings_grab(tThing, &cash);
  cash += 20'000'000; 
  hotelOfThings_drop(tThing.i, cash);
  expectExtinctSoon(50'000'000, __LINE__);
  return true;
}

bool testRob(void) {
  printf("testRob\n");
  make(5, 9000);
  Cash cash;
  hotelOfThings_grab(tThing, &cash);
  cash = 0; 
  hotelOfThings_drop(tThing.i, cash);
  expectExtinctSoon(0, __LINE__);
  return true;
}

bool testGod(void) {
  printf("testGod\n");
  make(6, 10'000'000);
  make(5, 0);
  Cash cash;
  expectExtinctSoon(10'000'000, __LINE__);
  printf("It's: %d\n", tocksNow());
  hotelOfThings_grab(tThing, &cash); //  hotelOfXXs_grab
  Cash expect =  - (hotelOfThings_rent()*(tocksNow()-FIRST_TOCK));
  assertLong(cash, expect);
  hotelOfThings_drop(tThing.i, cash);
  return true;
}

bool testHotel(void) { printf("Tock price: %f\n", tockPrice());
  printf("Billable size: %ld\n", billableThingSize);
  return
    //testNoPop() &&
    //test1() &&
    //testEarn() &&
    //testRob() &&
    testGod() &&
    //testAfterFree() &&
    //testBusy() &&
    //testFreeWhenBusy() &&
    // testMonkey() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }

