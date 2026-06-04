#pragma GCC diagnostic ignored "-Wunused-function"
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "types.h"
#include "test.h"
#include "core/h.h"
#include "misc/h.h"
#include "globals/h.h"
#include "work/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/h.h"
#include "Msg_hotel/structs.h"

#define SIZE_BOMB 32

#define MOB_PER_MSG_SIZE ( ( (double) (sizeof(Mob)+32)) / ( (double) (sizeof(Msg)+32)) ) 
#define MOB_CASH 400
#define BID 0.00006
#define PAY 100

double randProb() {
  uint64_t i = randIntBelow(1024);
  double f01 = ((double)i)/(1024.0);
  return (f01-0.5)*10 + 0.5;

}

bool makeOne() {
  Mob * pMob;
  MobIx i = hotelOfMobs.alloc(MOB_CASH, &pMob, 0);
  PhyB * p = &pMob->body.p.b;
  pMob->body.phylum = 'b';
  p->pay = PAY;
  p->bid = BID;
  p->spawnThresh = randIntBelow(800);
  p->fractionMsgOfMobSize = 1.0/MOB_PER_MSG_SIZE;
  //printf("fractionMsgOfMobSize = %f\n", p->fractionMsgOfMobSize);
  emit(pMob->rent.cash*p->fractionMsgOfMobSize, BID, i, pMob->rent.nick, i);
  return true;
}

bool setup() {
  for (int i = 0; i < 100; i++) makeOne();
  return true;
}

bool equi() {
  openGlobals();
  hotelOfMobs.open();
  raffleOfMsgs.open();

  setup();
  Worker * pW = thisWorker(0);
  runWorker(pW);

  raffleOfMsgs.close(DELETE);
  hotelOfMobs.close(DELETE);
  closeGlobals(DELETE);
  return true;
}

