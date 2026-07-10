#include "h.h"
#include "globals/api.h"
#include "Thing_hotel/ix.h"
#include "bit/Thing.h"
#include "Thing_hotel/api.h"

ThingTact tGod;

static void tock() {
  //hotelOfThings_raid();
}

static bool init(void) {
  onTestTock = tock;
  openGlobals();
  hotelOfThings_open(); //hotelOfThings_checkHotel(0);
  tGod = hotelOfThings_admit(0,true, 0,0,0); //God 
  return true;
}
 
void cleanupHotel(void) { hotelOfThings_close(Hide); closeGlobals(Hide); }

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
    // testMonkey() &&
    true;
}

bool hotel(void) { return bkt("hotel", init, testHotel, cleanupHotel); }

