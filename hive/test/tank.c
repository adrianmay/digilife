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

static void cleanup(void) { closeTank(HIDE); }

void train(MobBody * pMB) {
  pMB->phylum = PHY_B;
  pMB->p.b.spawnThresh = randIntBelow(500);
  pMB->p.b.payMsg = randIntBelow(100);
  pMB->p.b.bid = pow(1.4, (randIntBelow(10)-5));
}

static MsgIx postie(Cash cash, CpuBid bid, MobTact tSndr, MobTact tRcvr) {
  void stf(MsgPayload * pP) { }
  return post(50, 1, tSndr, tRcvr, stf);
}

static bool test1(void) {
  for (int i=0;i<500;i++) {
    MobTact t = spawn(500, tInvestor, train);
    postie(50, 1, t, t);
  }
  while (!shouldQuit)
    choose();
  return true;
}
static bool test(void) {
  return
    test1() &&
    true;
}

bool tank(void) { return bkt("tank", init, test, cleanup); }


