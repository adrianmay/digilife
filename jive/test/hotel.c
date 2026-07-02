#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#include <stdatomic.h>
#include <unistd.h>
#include "h.h"
#include "misc/api.h"
#include "globals/api.h"
#include "Thing_hotel/record.h"
#include "Thing_pile/ix.h"
#include "Thing_pile/api.h"
#include "Thing_hotel/api.h"

// ALL OFF COS OF PERF STUFF 

#define NOTIFY_TOCKS 150

void showThingBody(ThingBody * p) {
  printf("name=%d,code=<binary>\n", p->name);
}

static bool extinct=true;
void onThingsExtinct(void) { extinct = true; }
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
  tGod = hotelOfThings.admit(0,true, 0,0,0); //God
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
  tThing = hotelOfThings.admit(cash, false, stuff, &pThing, &recycledSlot);
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

bool with(ThingTact t, V_ThingP act) {
  Thing * pT = hotelOfThings.grab(t);
  if (pT) {
    act(pT);
    hotelOfThings.drop(t.i);
    return true;
  }
  else return false;
}

bool testEarn(void) {
  printf("testEarn\n");
  ThingTact t = make(4, 3000);
  void f(Thing * pT) { hotelOfThings.richer(pT, 2000); }
  with(t, f);
  expectExtinctSoon(5000);
  return true;
}

bool testRob(void) {
  printf("testRob\n");
  make(5, 9000);
  void f(Thing * pT) { hotelOfThings.poorer(pT, 0, Rob); }
  with(tThing, f);
  expectExtinctSoon(150);
  return true;
}

void colRent(Thing * p) {hotelOfThings.collectRent(p);}

bool testGod(void) {
  printf("testGod\n");
  make(6, 1000);
  make(5, 0);
  expectExtinctSoon(1000);
  with(tThing, colRent);
  bool ff(Thing * pThing) { assertLong(pThing->rent.cash, -1200l); return true;}
  void f(Thing * pThing) { ff(pThing); }
  with(tThing, f);
  return true;
}

bool testAfterFree(void) {
  printf("testAfterFree\n");
  ThingTact tThing = make(6, 1000);
  expectExtinctSoon(1000);
  void f(Thing * p) { (void)p; }
  Woth w = with(tThing, f);
  assertInt(w, Dead);
  return true;
}

bool testBusy(void) {
  printf("testBusy\n");
  ThingTact tThing = make(6, 10000);
  //printf("nick: %x\n", tThing.n);
  void f(Thing * p) { (void)p; }
  bool wi;
  void g(Thing * p) { 
    wi = with(tThing, f);
  }
  bool wo = with(tThing, g);
  assertInt(wo, true);
  assertInt(wi, false);
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
  bool wo = with(tThing, g);
  ThingTact t3 = make(8,1000);
  bool sameIndex3 = t3.i.i == tThing.i.i;
  assertInt(wo, true);
  assertInt(sameIndex2, false);
  assertInt(sameIndex3, true);
  return true;
}

#define MAX_THS 100
// #define Free 0
// #define Idle 1
// #define UsedBy 2
typedef enum {Free, Idle, UsedBy} St;
typedef struct { _Atomic St st; ThingTact t; Thing * p; } Th;
Th ths[MAX_THS]={0};

void onThingHotelGoDie(ThingIx i, Thing * pThing) { 
  St want = Idle;
  for (int a=0; a<10*MAX_THS; a++) {
    if (ths[a].t.i.i == i.i) {
      printf("GoDie %i\n", i.i);
      atomic_compare_exchange_strong(&ths[a].st, &want, Free);
    }
  }

}

void showTh(int a) {
  Th * p = ths+a;
  printf("st=%d, t=%d,%d, p=%p\n", atomic_load(&p->st), p->t.i.i, p->t.n, p->p);
}

int chSt(St from, St to) { 
  //printf("chSt %d->%d ... ", from, to);
  for (int i=0; i<10*MAX_THS; i++) {
    int a = randIntBelow(MAX_THS);
    St want = from;
    if (atomic_compare_exchange_strong(&ths[a].st, &want, to)) {
      //printf("returning: %d: was=%d \n", a, want); //showTh(a);
      return a; 
    }
  }
  return -1; 
}

void doit(int me, int dowhat) {
  printf("%d does %d\n", me, dowhat);
  switch (dowhat) {
    case 0: // Richer
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1)
          ths[i].p->rent.cash += randIntBelow(1000);
        break; }
    case 1: // Poorer
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1)
          ths[i].p->rent.cash -= randIntBelow(10+ths[i].p->rent.cash*1.1);
        break; }
    case 2: // Rob
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1)
          ths[i].p->rent.cash = 0;
        break; }

    case 3: // Grab
    case 4:
      { int i = chSt(Idle, UsedBy+me);
        if (i!=-1) {
          ths[i].p = hotelOfThings.grab(ths[i].t);
          if (!ths[i].p) atomic_store(&ths[i].st, Idle);
        }
        break; }
    case 5: // Drop
    case 6:
      { int i = chSt(UsedBy+me, Idle);
        if (i!=-1)
          hotelOfThings.drop(ths[i].t.i);
        break; }

    case 7:
    case 8:
      notifyCycles(randIntBelow(GUESS_CYCLES_PER_TOCK*2));
      break;

    case 9: // Admit
    case 10:
      { int i = chSt(Free, Idle);
        if (i!=-1) {
          Cash c = randIntBelow(1000);
          bool isGod = !randIntBelow(30);
          ths[i].t = hotelOfThings.admit(c, isGod, 0, 0, 0);
        }
        break; }
    case 11: // Collect rent
    case 12:
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1) 
          hotelOfThings.collectRent(ths[i].p);
        break; }
    case 13: // Raid
    case 14:
      { hotelOfThings.raid();
        break; }
  }
}

void * monkey(void * n) {
  int me = (int)( (int64_t) n);
  for (int a=0; a<10; a++) {
    doit(me, 9);  // Admit
    //hotelOfThings.show();
  }
  //while (true) {
  for (int a=0;a<1000;a++) {
//    hotelOfThings.show();
    doit(me, randIntBelow(15));
  }
  return 0;
}

#define NUM_THREADS 1
pthread_t pids[NUM_THREADS] = {0};

bool testMonkey(void) {
  for (int64_t a=0;a<NUM_THREADS; a++) pthread_create(pids+a, 0, monkey, (void*)a);
  for (int64_t a=0;a<NUM_THREADS; a++) pthread_join(pids[a], 0);
  return 0;
}
// Need grab vs raid race tests: 
//   Raid finds bomb during job
//   Job bankrupts rich mob
//   Lots of quick jobs, raid bombs in the middle, repeat many times
//   Alloc quickly into freed slot
//   Raid sees bombed already set
//   Charge rent on tocks during job


void cleanupHotel(void) { hotelOfThings.close(Hide); closeGlobals(Hide); }

bool testHotel(void) {
  printf("Tock price: %f\n", tockPrice());
  printf("Billable size: %ld\n", billableThingSize);
  return
    //testNoPop() &&
    //test1() &&
    //testEarn() &&
    //testRob() &&
    //testGod() &&
    //testAfterFree() &&
    //testBusy() &&
    //testFreeWhenBusy() &&
    testMonkey() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }


