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
  printf("Iter=%d Mobs=%d Msgs=%d \n", iter, hotelOfMobs.count(), raffleOfMsgs.count());
  //printf("spTh=%f payMsg=%f bid=%f\n", avg.spawnThresh, avg.payMsg, avg.bid);
  //hotelOfMobs.show();
  //raffleOfMsgs.show();
}

static void cleanup(void) { closeTank(HIDE); }


static void stuffMob(MobBody * pMB) { pMB->phylum = PHY_B; }
static void stuffPayload(MsgPayload * pP) { }

void eve(void) {
  Cash cash = 5000;
  Cash forSpawn = cash*MSG_PROP;
  Cash forPost = cash - forSpawn;

  Core core;
  core.tMob=tInvestor;
  core.mobCash = cash;
  core.msgCash = 0;
  core.bid=0;
  
  MobTact tEve;
  spawn(&core, forSpawn, stuffMob, &tEve); 
  post(&core, forPost, 0, tEve, stuffPayload);
}


static bool test1(void) {
  eve();
  printf("Eved\n");
  for (int a=0;a<406; a++) {
//  while (!shouldQuit) {
    //if (iter % 100 == 0) 
    //survey();
    choose();
    iter++;
  }
//  survey();
  hotelOfMobs.show();
  raffleOfMsgs.show();
  return true;
}
static bool test(void) {
  return
    test1() &&
    true;
}

bool tank(void) { return bkt("tank", init, test, cleanup); }


