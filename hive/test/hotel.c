#include "types.h"
#include <unistd.h>
#include "test.h"
#include "misc/h.h"
#include "perf/h.h"
#include "globals/h.h"
#include "Thing_hotel/structs.h"
#include "Thing_pile/1.h"
#include "Thing_hotel/h.h"
#include "bit/MobBody.h"

// ALL OFF COS OF PERF STUFF 

static bool extinct;
void onMobsExtinct() {}
void onMsgsExtinct() {}
void onThingsExtinct(void) { extinct = true; }
void onThingKilled(ThingIx i) {}
void showMobBody(MobIx i, MobBody * p) {
  printf("code=<binary>\n");
}

// 
// 
// 
// 
// Cycles cycles;
// Thing * pThing;
// ThingIx iThing;
// 
// static bool init(void) {
//   openGlobals();
//   hotelOfThings.open();
//   return true;
// }
// 
// void killTilExtinct(void) {
//   while (true) {
// //    printf("killTilExtinct: tocks=%d, processCycles=%'ld\n", tocksNow(), readProcessCycles());
//     hotelOfThings.kill();
//     //printf("GOGOGOG");
//     if (extinct) return;
//     sleepMs(10);
//   }
// }
// 
// bool testNoPop(void) {
//   extinct = false;
//   TIME_VOID_PROC(killTilExtinct());
//   printf("testNoPop: %'ld\n", cycles);
//   assertLongCond(cycles, <200000);
//   return true;
// }
// 
// void make(Ix name, Cash cash) {
//   bool recycledSlot;
//   iThing = hotelOfThings.alloc(cash, &pThing, &recycledSlot);
//   pThing->body.name = name;
//   hotelOfThings.show();
// }
// 
// bool test1(void) {
//   extinct = false;
//   make(3, 2000);
//   printf("Made in test1\n");
//   TIME_VOID_PROC(killTilExtinct());
//   printf("test1: %'ld\n", cycles);
//   assertLongCond(cycles, <2700000000ull)
//   assertLongCond(cycles, >2300000000ull)
//   return true;
// }
// 
// void * earn(void *) {
//   sleepNs(1000000000);
//   hotelOfThings.enrich(iThing, 2000);
//   return 0;
// }
// 
// bool testEarn(void) {
//   extinct = false;
//   printf("testEarn\n");
//   //hotelOfThings.show();
//   make(4, 3000);
//   //hotelOfThings.show();
//   background(earn);
//   TIME_VOID_PROC(killTilExtinct());
//   printf("testEarn: %'ld\n", cycles);
//   assertLongCond(cycles, <4000000000ull)
//   assertLongCond(cycles, >3600000000ull)
//   return true;
// }
// 
// void * rob(void *) {
//   sleepNs(250000000);
//   hotelOfThings.rob(iThing);
//   return 0;
// }
// 
// bool testRob(void) {
//   extinct = false;
//   printf("testRob\n");
//   make(5, 9000);
//   background(rob);
//   TIME_VOID_PROC(killTilExtinct());
//   printf("testRob: %'ld\n", cycles);
//   assertLongCond(cycles, <1400000000ull)
//   assertLongCond(cycles, > 900000000ull)
//   return true;
// }
// 
// 
// void cleanupHotel(void) { hotelOfThings.close(HIDE); closeGlobals(HIDE); }
// 
// bool testHotel(void) {
//   background(sweat_forever); // Got to do work to advance CPU time ...
//   return
//     testNoPop() &&
//     test1() &&
//     testEarn() &&
//     testRob() &&
//     true;
// }
// 

static bool init(void) { return true; }
bool testHotel(void) { return true; }
void cleanupHotel(void) { }
bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }


