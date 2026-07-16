#include "h.h"
#include "misc/api.h"
#include "globals/api.h"
#include "Thing_hotel/ix.h"
#include "bit/Thing.h"
#include "Thing_hotel/api.h"

static bool extinct=true;
void onThingHotel_extinct(void) { extinct = true; }
void onThingHotel_rentCollected(Cash cash) { }
void onThingHotel_rentDefaulted(Cash cash) { }
void onThingHotel_funeral(ThingIx, Thing * pThing) {}

//void onThingHotel_goDie(ThingIx i, Thing * pThing) {
//  St want = Idle;
//  for (int a=0; a<10*MAX_THS; a++) {
//    if (ths[a].t.i.i == i.i) {
//      printf("GoDie %i\n", i.i);
//      atomic_compare_exchange_strong(&ths[a].st, &want, Free);
//    }
//  }
//}

Cycles cycles;
Thing * pThing;
ThingTact tGod, tThing;

static void tock() { hotelOfThings_raid(); }

void showThing(ThingIx i, Thing * p) {
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

Tocks notifyCyclesTilExtinct(void) {
  Tocks started = tocksNow();
  while (true) {
//    printf("notifyCyclesTilExtinct: tocks=%d, processCycles=%'ld\n", tocksNow(), readProcessCycles());
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
  Tocks dur = notifyCyclesTilExtinct();
  Tocks expect;
  if (cash==0) 
    expect = NOTIFY_TOCKS;
  else {
    Tocks expectIdeal = cash / ( hotelOfThings_rec() * tockPrice() );
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
  printf("testNoPop\n");
  Tocks dur = notifyCyclesTilExtinct();
  assertCond(dur, ==0);
  return true;
}

bool test1(void) {
  printf("test1\n");
  Cash cash = 40'000'000;
  make(3, cash);
  expectExtinctSoon(cash, __LINE__);
  return true;
}

bool testEarn(void) {
  printf("testEarn\n");
  make(4, 30'000'000);
  Cash cash;
  hotelOfThings_grab(tThing, 0, &cash);
  cash += 20'000'000; 
  hotelOfThings_drop(tThing.i, cash);
  expectExtinctSoon(50'000'000, __LINE__);
  return true;
}

bool testRob(void) {
  printf("testRob\n");
  make(5, 9000);
  Cash cash;
  hotelOfThings_grab(tThing, 0, &cash);
  cash = 0; 
  hotelOfThings_drop(tThing.i, cash);
  expectExtinctSoon(0, __LINE__);
  return true;
}

bool testGod(void) {
  printf("testGod\n");
  make(6, 10'000'000);
  make(7, 0);
  Tocks start = tocksNow();
  Cash cash;
  expectExtinctSoon(10'000'000, __LINE__);
  printf("It's: %d\n", tocksNow());
  hotelOfThings_grab(tThing, 0, &cash); //  hotelOfXXs_grab
  Cash expect =  - (hotelOfThings_rent()*(tocksNow()-start));
  assertLong(cash, expect);
  hotelOfThings_drop(tThing.i, cash);
  return true;
}

bool testAfterFree(void) {
  printf("testAfterFree\n");
  ThingTact tThing = make(8, 10'000'000);
  expectExtinctSoon(10'000'000, __LINE__);
  Cash cash;
  Thing * p;
  hotelOfThings_grab(tThing, &p, &cash);
  hotelOfThings_drop(tThing.i, cash);
  assertCond((int)(long)p, ==0);
  return true;
}

bool testBusy(void) {
  printf("testBusy\n");
  make(9, 10'000'000);
  Cash cash;
  hotelOfThings_grab(tThing, &pThing, &cash);
  assertCond((int)(long)pThing, !=0);
  hotelOfThings_grab(tThing, &pThing, &cash);
  assertCond((int)(long)pThing, ==0);
  hotelOfThings_drop(tThing.i, cash);
  return true;
}

// We make a thing go bankrupt while its busy and assert
// that the slot won't be reused until it stops being busy.
bool testFreeWhenBusy(void) {
  printf("testFreeWhenBusy\n");
  ThingTact tThing = make(10, hotelOfThings_rent());
  bool sameIndex2;
  Cash cash;
  hotelOfThings_grab(tThing, &pThing, &cash);
  notifyCycles(2*GUESS_CYCLES_PER_TOCK); 
  ThingTact t2 = make(11, 1'000'000);
  sameIndex2 = t2.i.i == tThing.i.i;
  cash = 0;
  hotelOfThings_drop(tThing.i, cash);
  ThingTact t3 = make(8,1000);
  bool sameIndex3 = t3.i.i == tThing.i.i;
  assertInt(sameIndex2, false);
  assertInt(sameIndex3, true);
  return true;
}

/////////////////////////////////////////////////////////////////
// Monkey test

#define MAX_THS 100
typedef enum {Free, Idle, UsedBy} St;
typedef struct { _Atomic St st; ThingTact t; Thing * p; Cash c;  } Th;
Th ths[MAX_THS]={0};

void onThingHotel_goDie(ThingIx i, Thing * pThing) { 
  St want = Idle;
  for (int a=0; a<10*MAX_THS; a++) {
    if (ths[a].t.i.i == i.i) {
      printf("GoDie %i\n", i.i);
      atomic_compare_exchange_strong(&ths[a].st, &want, Free);
    }
  }
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

void doit(int me, int dowhat, int it) {
  //printf("At %d, %d does %d\n", it, me, dowhat);
  switch (dowhat) {
    case 0: // Richer
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1)
          ths[i].c += randIntBelow(1000);
        break; }
    case 1: // Poorer
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1)
          ths[i].c -= randIntBelow(10+ths[i].c*1.1);
        break; }
    case 2: // Rob
      { int i = chSt(UsedBy+me, UsedBy+me);
        if (i!=-1)
          ths[i].c = 0;
        break; }

    case 3: // Grab
    case 4:
      { int i = chSt(Idle, UsedBy+me);
        if (i!=-1) {
          hotelOfThings_grab(ths[i].t, &ths[i].p, &ths[i].c);
          if (!ths[i].p) atomic_store(&ths[i].st, Idle);
        }
        break; }
    case 5: // Drop
    case 6:
      { int i = chSt(UsedBy+me, Idle);
        if (i!=-1)
          hotelOfThings_drop(ths[i].t.i, ths[i].c);
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
          ths[i].t = hotelOfThings_admit(c, isGod, 0, 0, 0);
        }
        break; }
    case 11: // Raid
    case 12:
      { hotelOfThings_raid();
        break; }
  }
}
_Atomic int iterations;

void * monkey(void * n) {
  int me = (int)( (int64_t) n);
  for (int a=0; a<10; a++) {
    doit(me, 9, 0);  // Admit
    //hotelOfThings_show();
  }
  while (true) {
    //printf("it=%d things=%d\n", a, hotelOfThings_count() );
//    hotelOfThings_show();
    int it = atomic_load(&iterations);
    if (it>=100000) break;
    doit(me, randIntBelow(13), it);
    atomic_fetch_add(&iterations, 1);
  }
  return 0;
}

#define NUM_THREADS 20
pthread_t pids[NUM_THREADS] = {0};

bool testMonkey(void) {
  atomic_store(&iterations, 0);
  for (int64_t a=0;a<NUM_THREADS; a++) pthread_create(pids+a, 0, monkey, (void*)a);
  for (int64_t a=0;a<NUM_THREADS; a++) pthread_join(pids[a], 0);
  int i = atomic_load(&iterations);
  printf("Finished %'d iterations\n", i );
  return 0;
}

bool testHotel(void) { printf("Tock price: %f\n", tockPrice());
  printf("Billable size: %f\n", hotelOfThings_rec());
  return
    testNoPop() &&
    test1() &&
    testEarn() &&
    testRob() &&
    testGod() &&
    testAfterFree() &&
    testBusy() &&
    testFreeWhenBusy() &&
    testMonkey() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }

