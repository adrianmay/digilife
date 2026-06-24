#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "test.h"
#include "tank/h.h"
#include "misc/h.h"


static bool shouldQuit = false;
void onMobsExtinct() { printf("onMobsExtinct\n"); shouldQuit = true; }
void onMsgsExtinct() {}

static void tock() {
  //printf("Tock\n");
  hotelOfMobs.raid();
}

static bool init(void) { 
  onTestTock = tock;
  openTank();  
  return true; 
}


void survey() {
  printf("Iter=%d Mobs=%d Msgs=%d ", iter, hotelOfMobs.count(), raffleOfMsgs.count());
  //printf("spTh=%f payMsg=%f bid=%f\n", avg.spawnThresh, avg.payMsg, avg.bid);
  //hotelOfMobs.show();
  //raffleOfMsgs.show();
}

static void cleanup(void) { closeTank(HIDE); }

static void train(MobBody * pMB) {
 pMB->phylum = PHY_B;
 //pMB->p.b.spawnThresh = randIntBelow(500);
 //pMB->p.b.payMsg = 0;
 //pMB->p.b.bid = 0; //0.01 * pow(1.4, (randIntBelow(10)-5));
// pMB->phylum = PHY_B;
// pMB->p.b.spawnThresh = randIntBelow(500);
// pMB->p.b.payMsg = 100 + randIntBelow(100);
// pMB->p.b.bid = 0; //0.01 * pow(1.4, (randIntBelow(10)-5));
}

static void postie(Cash cash, CpuBid bid, MobTact tSndr, MobTact tRcvr) {
  void stf(MsgPayload * pP) { }
  void f(Mob * pMob) { post(cash, 1, pMob, tSndr, tRcvr, stf); }
  hotelOfMobs.with(tSndr, f);
}

static bool test1(void) {
  //for (int i=0;i<500;i++) {
  //survey();
  MobTact t;
  void f(Mob * pInv) { t = spawn(8000, pInv, tInvestor, train); }
  hotelOfMobs.with(tInvestor, f);
  //survey();
  postie(2000, 1, t, t);
  //printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
//  for (int a=0;a<10;a++) {
  while (!shouldQuit) {
    if (iter % 1000 == 0) 
      survey();
    choose();
    iter++;
  }
  survey();
  hotelOfMobs.show();
//  raffleOfMsgs.show();
  return true;
}
static bool test(void) {
  return
    test1() &&
    true;
}

bool tank(void) { return bkt("tank", init, test, cleanup); }


