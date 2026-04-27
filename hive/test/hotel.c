#include "types.h"
bool hotel() { return true; }

void onMobsExtinct() {}

//#include "test.h"
//#include "misc/h.h"
//#include "ThingBulk_pile/1.h"
//#include "ThingBomb_pile/1.h"
//#include "Thing_hotel/Bulk.h"
//#include "Thing_hotel/h.h"
//
//Nanosecs ns;
//ThingBulk * pThing;
//ThingBulkIndex iThing;
//
//static bool init() {
//  openGlobals();
//  hotelOfThings.open();
//  background(sweat_forever); // Got to do work to advance CPU time ...
//  return true;
//}
//
//bool testNoPop() {
//  TIME_VOID_PROC(hotelOfThings.collectRent(0)); 
//  printf("testNoPop: %'ld\n", ns);
//  assertLongCond(ns, <4000);
//  return true;
//}
//
//
//void make(Index name, Cash cash) {
//  hotelOfThings.alloc(cash, &pThing);
//  pThing->body.name = name;
//}
//
//bool test1() {
//  make(3, 2000);
//  TIME_VOID_PROC(hotelOfThings.collectRent(0));
//  assertLongCond(ns, <2100000000ull)
//  assertLongCond(ns, >1900000000ull)
//  return true;
//}
//
//static bool testHotel() {
//  return 
//    //testNoPop() &&
//    test1() &&
////    testEarn() &&
//    true;
//}
//
//static void cleanupHotel() { closeGlobals(1); hotelOfThings.close(1); }
//bool hotel() { return bkt("hotel", init, testHotel, cleanupHotel); }

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
  /*
   *
#include "test.h"
#include "hotel/h.h"

Nanosecs ns;

MAKEHOTEL1(Block)

typedef struct { Index name; BlockRent rent; } Block;

void BlockFuneral(Block * pB) {
  printf("Alas poor BlockMeap %d, I knew him well.\n", pB->name);
}
MAKEHOTEL2(Block, GIGA)


bool init() {
  openGlobals();
  openBlockHotel();
  background(sweat_forever); // Got to do work to advance CPU time ...
  return true;
}


Block * pB; 
BlockMeapIndex iBM;

void make(Index name, Cash cash) {
  reserveBlock(cash, &pB);
  pB->name = name;
}

bool test1() {
  make(3, 2000);
  TIME_VOID_PROC(BlockRentCollector());
  assertLongCond(ns, <2100000000ull)
  assertLongCond(ns, >1900000000ull)
  return true;
}

void * earn(void *) {
  sleepS_(1);
  pB->rent.cash += 2000;
  meapReviewBlockMeap(iBM);
  return 0;
}

bool testEarn() {
  make(4, 2000);
  background(earn);
  TIME_VOID_PROC(BlockRentCollector());
  assertLongCond(ns, <4100000000ull)
  assertLongCond(ns, >3900000000ull)
  return true;
}

bool testHotel() {
  return 
    testNoPop() &&
    test1() &&
    testEarn() &&
    true;
}

void cleanupHotel() { closeGlobals(1); closeBlockHotel(1); }
bool hotel() { return bkt("hotel", init, testHotel, cleanupHotel); }

*/
