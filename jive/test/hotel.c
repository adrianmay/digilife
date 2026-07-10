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

#define NOTIFY_TOCKS 154

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

bool expectExtinctSoon(Cash cash) {
  Tocks dur = killTilExtinct();
  Tocks expect;
  if (cash==0) 
    expect = 0;
  else {
    Tocks expectIdeal = cash / ( billableThingSize * tockPrice() );
    printf("expectIdeal=%d\n", expectIdeal);
    expect = NOTIFY_TOCKS + ((expectIdeal - 1) / NOTIFY_TOCKS + 1) * NOTIFY_TOCKS ;
  }
  assertInt(dur, expect);
  return true;
}

ThingTact make(Ix name, Cash cash) {
  bool recycledSlot;
  void stuff(Thing * p) { p->name = name; }
  tThing = hotelOfThings_admit(cash, false, stuff, &pThing, &recycledSlot);
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
  Cash cash = 40000000;
  make(3, cash);
  printf("Made in test1\n");
  expectExtinctSoon(cash);
  return true;
}

bool testHotel(void) { printf("Tock price: %f\n", tockPrice());
  printf("Billable size: %ld\n", billableThingSize);
  return
    testNoPop() &&
    test1() &&
    //testEarn() &&
    //testRob() &&
    //testGod() &&
    //testAfterFree() &&
    //testBusy() &&
    //testFreeWhenBusy() &&
    // testMonkey() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }

