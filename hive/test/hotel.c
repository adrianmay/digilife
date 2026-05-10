#include "types.h"
#include <unistd.h>
#include "test.h"
#include "misc/h.h"
#include "globals/h.h"
#include "Thing_hotel/Bulk.h"
#include "Thing_hotel/h.h"

static bool extinct;
void onThingsExtinct(void) { extinct = true; } 

// void onMobsExtinct() {}
// void onMsgsExtinct() {}

Nanosecs ns;
ThingBulk * pThing;
ThingBulkIndex iThing;
static ThingBulkIndex iDonor = {0};


static bool init(void) {
  openGlobals();
  hotelOfThings.open(1000000, &iDonor);
  return true;
}

void killTilExtinct(void) {
  while (true) {
    updateTocks();
    printf("killTilExtinct: now=%d\n", tocksNow());
    hotelOfThings.kill(); 
    if (extinct) return;
    usleep(50000);
  }
}

bool testNoPop(void) {
  hotelOfThings.show();
  TIME_VOID_PROC(killTilExtinct()); 
  printf("testNoPop: %'ld\n", ns);
  assertLongCond(ns, <20000);
  return true;
}

void make(Index name, Cash cash) {
  bool recycledSlot;
  iThing = hotelOfThings.alloc(cash, iDonor, &pThing, &recycledSlot);
  pThing->body.name = name;
}

bool test1(void) {
  extinct = false;
  make(3, 2000);
  hotelOfThings.show();
  TIME_VOID_PROC(killTilExtinct());
  printf("test1: %'ld\n", ns);
  assertLongCond(ns, <2100000000ull)
  assertLongCond(ns, >1900000000ull)
  return true;
}

void * earn(void *) {
  sleepNs(1000000000);
  hotelOfThings.transfer(2000, iDonor, iThing);
  //hotelOfThings.review(iThing);
  return 0;
}

bool testEarn(void) {
  make(4, 3000);
  background(earn);
  TIME_VOID_PROC(killTilExtinct());
  assertLongCond(ns, <5100000000ull)
  assertLongCond(ns, >4900000000ull)
  return true;
}

void cleanupHotel(void) { hotelOfThings.close(DELETE); closeGlobals(DELETE); }

bool testHotel(void) {
  background(sweat_forever); // Got to do work to advance CPU time ...
  return 
    //testNoPop() &&
    //test1() &&
    //(cleanupHotel(), init()) &&
    //test1() &&
    //(cleanupHotel(), init()) &&
    testEarn() &&
    //testRob() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }

//
//static bool init() {
//  openGlobals();
//  hotelOfThings.open();
//  background(sweat_forever); // Got to do work to advance CPU time ...
//  return true;
//}
//
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


//bool testNoPop() {
//  TIME_VOID_PROC(hotelOfThings.collectRent(0)); 
//  printf("testNoPop: %'ld\n", ns);
//  assertLongCond(ns, <4000);
//  return true;
//}

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

*/

