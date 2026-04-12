#include "test.h"
#include "hotel.h"

Nanosecs ns;

MAKERENT1(Block)

typedef struct { Index name; BlockRent rent; } Block;

MAKERENT2(Block, GIGA)

void mournBlockMeap(BlockMeap * pM) {
  printf("Alas poor BlockMeap %d, I knew him well.\n", getBlock(pM->who)->name);
}

bool init() {
  openGlobals();
  openBlockHotel();
  background(sweat_forever); // Got to do work to advance CPU time ...
  return true;
}

bool testNoPop() {
  TIME_VOID_PROC(rentCollector(killBlocks)); assertLongCond(ns, <5000)
  return true;
}

Block * pB; 
BlockMeapIndex iBM;

void make(Index name, Cash cash) {
  BlockIndex iB; 
  iB = allocBlock(&pB);
  pB->name = name;
  pB->rent.cash = cash;
  BlockMeap * pM;
  meapInsertBlockMeap(&iBM, &pM, iB.i);
}

bool test1() {
  make(3, 2000);
  TIME_VOID_PROC(rentCollector(killBlocks));
  assertLongCond(ns, <2100000000ull)
  assertLongCond(ns, >1900000000ull)
  return true;
}

void * earn(void *) {
  sleepS(1);
  pB->rent.cash += 2000;
  meapReviewBlockMeap(iBM);
  return 0;
}

bool testEarn() {
  make(4, 2000);
  background(earn);
  TIME_VOID_PROC(rentCollector(killBlocks));
  assertLongCond(ns, <4100000000ull)
  assertLongCond(ns, >3900000000ull)
  return true;
}

bool testRent() {
  return 
    testNoPop() &&
    test1() &&
//    testEarn() &&
    true;
}

void cleanupRent() { closeGlobals(1); closeBlockHotel(1); }
bool rent() { return bkt(init, testRent, cleanupRent); }

