#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "test.h"
#include "tank/h.h"
#include "misc/h.h"


static bool shouldQuit = false;
void onMobsExtinct() { shouldQuit = true; }
void onMsgsExtinct() {}

static void tock() {
  hotelOfMobs.raid();
}

static bool init(void) { 
  onTestTock = tock;
  openTank();  
  return true; 
}

void survey() {
  printf("##############################################\nMobs=%d, Msgs=%d\n", hotelOfMobs.count(), raffleOfMsgs.count());
  hotelOfMobs.show();
  raffleOfMsgs.show();
}
static void cleanup(void) { closeTank(HIDE); }

void train(MobBody * pMB) {
  pMB->phylum = PHY_B;
  pMB->p.b.spawnThresh = randIntBelow(500);
  pMB->p.b.payMsg = 100 + randIntBelow(100);
  pMB->p.b.bid = 1; //0.01 * pow(1.4, (randIntBelow(10)-5));
}

static MsgIx postie(Cash cash, CpuBid bid, MobTact tSndr, MobTact tRcvr) {
  void stf(MsgPayload * pP) { }
  return post(cash, 1, tSndr, tRcvr, stf);
}

static bool test1(void) {
  //for (int i=0;i<500;i++) {
  //survey();
  MobTact t = spawn(2000, tInvestor, train);
  //survey();
  postie(100, 1, t, t);
  //printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
  while (!shouldQuit) {
    //raffleOfMsgs.show();
    //survey();
    choose();
  }
  survey();
  return true;
}
static bool test(void) {
  return
    test1() &&
    true;
}

bool tank(void) { return bkt("tank", init, test, cleanup); }


