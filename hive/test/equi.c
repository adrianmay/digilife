#pragma GCC diagnostic ignored "-Wunused-function"
#include <unistd.h>
#include <stdio.h>
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

#define MOB_PER_MSG_SIZE ( ((double)sizeof(Mob))/((double)sizeof(Msg)) ) // TODO: Approx, what about bombs.
#define MOB_CASH 300
#define BID 0.01
#define PAY 200

double randProb() {
  uint64_t i = randIntBelow(1024);
  return ((float)i)/(1024.0);
}

bool makeOne() {
  Mob * pMob;
  MobIx i = hotelOfMobs.alloc(MOB_CASH, &pMob, 0);
  PhyB * p = &pMob->body.p.b;
  pMob->body.phylum = 'b';
  p->spawnCash = MOB_CASH;
  p->pay = PAY;
  p->bid = BID;
  p->oddsLazyByCash0 = randProb();
  p->oddsLazyByCash1 = randProb();
  p->msgCashByCash0  = randProb();
  p->msgCashByCash1  = randProb();
  p->fractionMsgOfMobSize = 1.0/MOB_PER_MSG_SIZE;
  //printf("fractionMsgOfMobSize = %f\n", p->fractionMsgOfMobSize);
  emit(pMob->rent.cash*p->fractionMsgOfMobSize, BID, i, i);
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

