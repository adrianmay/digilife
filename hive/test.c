#include <stdio.h>
#include <stdlib.h>
#include "pile.h"
#include "meap.h"

#define assertInt(VAR, VAL) \
  if (VAR != VAL) { \
    printf("FAILED at %d: Expected: " #VAL "; Got: %d\n", __LINE__, VAR); \
    cleanup();  \
    exit(1); \
  }
#define assertInt_(VAR, VAL, CLEANUP) \
  if (VAR != VAL) { \
    printf("FAILED at %d: Expected: " #VAL "; Got: %d\n", __LINE__, VAR); \
    CLEANUP(); \
    exit(1); \
  }

typedef struct __attribute__((aligned(KILO))) { 
  int fd;
  uint64_t epochTimeStarted; 
  uint64_t epochTimeStopped; 
  uint64_t epochTimeTock; 
} Globals;

MAKEGLOBALS

MAKEPILE1(Thing);

typedef struct __attribute__((aligned(KILO))) { 
  int x;
  ThingIndex next;
} Thing;

Thing prototypeThing = { 0, (ThingIndex) {BAD_INDEX} };
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
  closeMyMeapPile(false); //Delete it for next time
  hideThingPile();                      
  hideMyMeapPile();                      
  closeGlobals(true);
}

void globals() {
  bool v = openGlobals();
  assertInt_(v, true, cleanup_globals);
  uint64_t i = g->epochTimeStarted + g->epochTimeStopped + g->epochTimeTock;
  assertInt_(i, 0, cleanup_globals);
  g->epochTimeStarted = 1;
  g->epochTimeStopped = 2;
  g->epochTimeTock = 3;
  closeGlobals(false);
  v = openGlobals();
  assertInt_(v, false, cleanup_globals);
  i = g->epochTimeStarted + g->epochTimeStopped + g->epochTimeTock;
  assertInt_(i, 6, cleanup_globals);
  closeGlobals(true);
}

void virginity() {
  bool vir1 = openThingPile(); //Assume it doesn't exist
  //assertInt(vir1,true);
  ThingIndex i = allocThing();
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
  bool vir3 = openThingPile(); //Assume it doesn't exist
  assertInt(vir3,true);
  ThingIndex i0, i;
  i0 = i = allocThing(); // Must be index zero
  Thing * pT = getThing(i);
  for (int a=0;a<1000;a++) {
    pT->next=allocThing();
    pT = getThing(pT->next);
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
  ThingIndex i = allocThing();
  assertInt(i.i,3);
  i = allocThing();
  assertInt(i.i,4);
  i = allocThing();
  assertInt(i.i,2);
  i = allocThing();
  assertInt(i.i,1001);
  int count = countThings();
  assertInt(count,1002);
}

int pile() { 
  printf("Running basic pile tests\n"); 
  virginity();
  ThingIndex i = sumitems();
  freeing(i);
  reallocing();
  return 0; 
}

Score scoreMyMeap(MyMeap * p) {return p->tocks; }
void onMoveMyMeap(MyMeap *, MyMeapIndex) {}  
void onNewLowMyMeap(Score s) {}

void assertWholeMeap(Index * p, int n) {
  Index tot = getUsr(headOfMyMeaps);
  assertInt(tot, n);
  for (int a = 0; a<n; a++) {
    MyMeap * v = getMyMeap((MyMeapIndex) {a});
    assertInt(v->tocks,p[a]);
  }

}

int meap() {
  openMyMeapPile();
  meapInsertMyMeap((MyMeap) { 0x30 });
  meapInsertMyMeap((MyMeap) { 0x20 });
  meapInsertMyMeap((MyMeap) { 0x50 });
  meapInsertMyMeap((MyMeap) { 0x10 });
  meapInsertMyMeap((MyMeap) { 0x08 });
  meapInsertMyMeap((MyMeap) { 0x18 });
  meapRemoveMyMeap((MyMeapIndex){0});
  Index exp1[] = {0x10, 0x20, 0x18, 0x30, 0x50};
  assertWholeMeap(exp1, 5);
  meapRemoveMyMeap((MyMeapIndex){0});
  Index exp2[] = {0x18, 0x20, 0x50, 0x30};
  assertWholeMeap(exp2, 4);
}

MAKERENT1(Block)

typedef struct { Index name; BlockRent rent; } Block;
Block prototypeBlock = {0, { 0, 0, (BlockMeapIndex) {BAD_INDEX}}};
BlockMeap prototypeBlockMeap = {0, (BlockIndex) {BAD_INDEX}};

MAKERENT2(Block, GIGA)

int mortal() {

}

int main() { 
  globals();
  pile();
  meap();
  mortal();
  cleanup();
  return 0;
}
