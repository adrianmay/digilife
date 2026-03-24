#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "rent.h"

#define assertInt_(VAR, VAL, CLEANUP) \
  if (VAR != VAL) { \
    fprintf(stderr, "FAILED at %d: Expected: " #VAL "=%d ; Got: %d\n", __LINE__, VAL, VAR); \
    CLEANUP(); \
    exit(1); \
  }

void cleanup_nothing() {}
#define assertInt0(VAR, VAL) assertInt_(VAR, VAL, cleanup_nothing)

void wrap() {
  uint8_t z;
  z = wrapSubtract8(3,1);     assertInt0(z,2);
  z = wrapSubtract8(10,250);  assertInt0(z,16);
  z = wrapSubtract8(250,10);  assertInt0(z,240);
  z = wrapSubtract8(250,255); assertInt0(z,251);
  z = wrapAdd8(250,250);      assertInt0(z,244);
  z = wrapAdd8(250,10);       assertInt0(z,4);
}

void handler(union sigval sv) {}

void now() {
  int r;
  timer_t tid;
  struct sigevent se = { 
    .sigev_notify = SIGEV_THREAD, 
    .sigev_notify_function = handler,
      
  };
  r = timer_create(CLOCK_PROCESS_CPUTIME_ID, &se, &tid);
  assertInt_(r,0,cleanup_nothing);
  r = timer_create(CLOCK_THREAD_CPUTIME_ID, &se, &tid);
  assertInt_(r,0,cleanup_nothing);
  r = timer_create(CLOCK_THREAD_CPUTIME_ID, &se, &tid);
  assertInt_(r,0,cleanup_nothing);
  r = timer_create(CLOCK_THREAD_CPUTIME_ID, &se, &tid);
  assertInt_(r,0,cleanup_nothing);

//  uint64_t z=1;
//  struct timespec ts;
//  uint64_t start, now;
//  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts); now = ts.tv_sec*1000000 + ts.tv_nsec/1000;
//  start=now;
//  for (int a=0;a<99999999;a++) z*=a;
//  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts); now = ts.tv_sec*1000000 + ts.tv_nsec/1000;
//  printf("USED: %f\n", (((double)(now-start))/1000000.0));
//  for (int a=0;a<99999999;a++) z*=a;
//  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts); now = ts.tv_sec*1000000 + ts.tv_nsec/1000;
//  printf("USED: %f\n", (((double)(now-start))/1000000.0));
//  for (int a=0;a<99999999;a++) z*=a;
//  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts); now = ts.tv_sec*1000000 + ts.tv_nsec/1000;
//  printf("USED: %f\n", (((double)(now-start))/1000000.0));
//  for (int a=0;a<99999999;a++) z*=a;
//  int z=1;
//  realTime();
//  printf("%ld\n", CLOCKS_PER_SEC);
//  for (int a=0;a<999999999;a++) z*=a;
//  printf("REAL: %ld\n", realTime());
//  printf("CPU : %ld\n", ticksUsed());
//  for (int a=0;a<999999999;a++) z*=a;
//  printf("REAL: %ld\n", realTime());
//  printf("CPU : %ld\n", ticksUsed());
}

#define assertInt(VAR, VAL) assertInt_(VAR, VAL, cleanup)

MAKEGLOBALS

MAKEPILE1(Thing);

typedef struct __attribute__((aligned(KILO))) { 
  int x;
  ThingIndex next;
} Thing;

MAKEPILE2(Thing, GIGA);

MAKEMEAP1(MyMeap)

typedef struct { Tocks tocks; } MyMeap;

MyMeap prototypeMyMeap = { 0 };

MAKEMEAP2(MyMeap, GIGA)

void cleanup_globals() {
  closeGlobals(true);
}

void cleanup() {
  closeThingPile(false); //Delete it for next time
  hideThingPile();                      
  closeMyMeapPile(false); //Delete it for next time
  hideMyMeapPile();                      
  closeGlobals(true);
}

void globals() {
  bool v = openGlobals();
  assertInt_(v, true, cleanup_globals);
  uint64_t i = g->lastKnownTock + g->tocksReviewedAt + g->nsPerTock;
  assertInt_(i, 0, cleanup_globals);
  g->lastKnownTock = 1;
  g->tocksReviewedAt = 2;
  g->nsPerTock = 3;
  closeGlobals(false);
  v = openGlobals();
  assertInt_(v, false, cleanup_globals);
  i = g->lastKnownTock + g->tocksReviewedAt + g->nsPerTock;
  assertInt_(i, 6, cleanup_globals);
  closeGlobals(true);
}

void virginity() {
  bool vir1 = openThingPile(); //Assume it doesn't exist
  //assertInt(vir1,true);
  ThingIndex i = allocThing(0);
  closeThingPile(false); //Don't delete the pile
  bool vir2 = openThingPile();
  assertInt(vir2,false);
  closeThingPile(true); //Delete it for next time
}

int sumThings(ThingIndex i0) {
  Thing * pT;
  int total=0;
  for (ThingIndex i=i0;validThingIndex(i);i=pT->next) { pT = getThing(i); total += pT->x;}
  return total;
}

ThingIndex sumitems() {
  bool vir = openThingPile(); //Assume it doesn't exist
  Thing * pT;
  assertInt(vir,true);
  ThingIndex i0, i;
  i0 = i = allocThing(&pT); // Must be index zero
  pT->x = 0; 
  for (int a=0;a<1000;a++) {
    pT->next=allocThing(&pT);
    pT->next = badThingIndex;                       
    pT->x = 10*a;
  }
  int total = sumThings(i0);
  assertInt(total,4995000);
  return i0;
}

ThingIndex nextThing(ThingIndex i) { return getThing(i)->next; }

void freeing(ThingIndex i0) {
  int count = countThings();
  assertInt(count,1001);
  ThingIndex i1 = nextThing(i0);
  ThingIndex i2 = nextThing(i1);
  ThingIndex i3 = nextThing(i2);
  ThingIndex i4 = nextThing(i3);
  ThingIndex i5 = nextThing(i4);
  freeThing(i2);
  freeThing(i4);
  freeThing(i3);
  getThing(i1)->next = i5;
  int total = sumThings(i0);
  assertInt(total,4994940);
  count = countThings();
  assertInt(count,998);
}

void reallocing() {
  ThingIndex i = allocThing(0);
  assertInt(i.i,3);
  i = allocThing(0);
  assertInt(i.i,4);
  i = allocThing(0);
  assertInt(i.i,2);
  i = allocThing(0);
  assertInt(i.i,1001);
  int count = countThings();
  assertInt(count,1002);
}

int pile() { 
  printf("Running basic pile tests\n"); 
  virginity();
  ThingIndex i0 = sumitems();
  freeing(i0);
  reallocing();
  return 0; 
}

Score getScoreMyMeap(MyMeap * p) {return p->tocks; }
void onNewMyMeap(MyMeapIndex i, uint32_t hint) { getMyMeap(i)->tocks = hint; }  
void onMoveMyMeap(MyMeap *, MyMeapIndex) {}  
void onNewLowMyMeap(Score s) {}

void assertWholeMeap(Index * pExp, int n) {
  Index tot = getUsr(headOfMyMeaps);
  assertInt(tot, n);
  for (int a = 0; a<n; a++) {
    MyMeap * v = getMyMeap((MyMeapIndex) {a});
    assertInt(v->tocks,pExp[a]);
  }
}


int meap() {
  MyMeap * pMeap;
  openMyMeapPile();
  meapInsertMyMeap(&pMeap, 0x30);
  meapInsertMyMeap(&pMeap, 0x20);
  Index exp3[] = {0x20, 0x30};
  assertWholeMeap(exp3, 2);
  meapInsertMyMeap(&pMeap, 0x50);
  meapInsertMyMeap(&pMeap, 0x10);
  meapInsertMyMeap(&pMeap, 0x08);
  meapInsertMyMeap(&pMeap, 0x18);
  meapRemoveMyMeap((MyMeapIndex){0});
  Index exp1[] = {0x10, 0x20, 0x18, 0x30, 0x50};
  assertWholeMeap(exp1, 5);
  meapRemoveMyMeap((MyMeapIndex){0});
  Index exp2[] = {0x18, 0x20, 0x50, 0x30};
  assertWholeMeap(exp2, 4);
}

MAKERENT1(Block)

typedef struct { Index name; BlockRent rent; } Block;

MAKERENT2(Block, GIGA)

int rent() {
}

int main() { 
  wrap();
  globals();
  pile();
  meap();
  rent();
//  now();
  cleanup();
  return 0;
}

 
