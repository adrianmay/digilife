#include "Mob_pile/1.h"
#include "Msg_pile/1.h"
#include "Mob_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "globals/h.h"
#include "h.h"

void onMobKilled(MobIx i) {
  pileOfMobs.free(i);
}

Weight bidToWeight(CpuBid bid) {return 10000000*bid;}

void showTank() {
  hotelOfMobs.show();
  raffleOfMsgs.show();
}

MobTact tact(MobIx i) {
  Ix nick = hotelOfMobs.get(i)->rent.nick;
  if (nick & 0x80000000) abort();
  return (MobTact) {i, nick};
}

bool checkTact(MobTact t) {
  Mob * p = hotelOfMobs.get(t.i);
  return t.n == p->rent.nick;
}

void checkTactAbort(MobTact t) {
  if (!checkTact(t)) abort();
}

Mob * derefTact(MobTact t) {
  Mob * p = hotelOfMobs.get(t.i);
  if (p->rent.nick == t.n)
    return p;
  else 
    return 0;
}

MobTact tGod;

void makeGod() {
  Mob * p;
  MobIx iChild = hotelOfMobs.alloc(0, &p, 0);
  if (iChild.i != 0) abort(); //Only one god.
  tGod = (MobTact){iChild,p->rent.nick};
}

MobTact spawn(Cash cash, MobTact tParent, WithBody train) {
  MobIx iChild = badMobIx;
  if (checkTact(tParent) && hotelOfMobs.chargeIfCan(tParent.i, cash)) {
    Mob * pMob;
    iChild = hotelOfMobs.alloc(cash, &pMob, 0);
    train(&pMob->body);
  }
  return tact(iChild);
}


MsgIx post(Cash cash, CpuBid bid, MobTact tS, MobTact tR) {
  MsgTicket tkt;
  tkt.cpuBid = bid;
  tkt.rcvr = tR;
  tkt.sndr = tS;
  if (checkTact(tS) && hotelOfMobs.chargeIfCan(tS.i, cash))
    return raffleOfMsgs.enter(cash, bidToWeight(bid), &tkt);
  return badMsgIx;
}

DeliverResult deliver(Runner runner, Msg * pMsg) {
  MobTact tMob = pMsg->body.ticket.rcvr;
  Mob * pMob = derefTact(tMob);
  if (!pMob) { 
    // TODO: Pay to intestate msg death account
    return (DeliverResult) {false}; 
  }
  CostAndResult car = runner(pMob);
  if (car.cost>0) { // Not overworked so give change to rcvr
    hotelOfMobs.enrich(tMob.i, pMsg->rent.cash - car.cost);
  }
  return (DeliverResult){true, car.res};
}

bool openTank() {
  openGlobals();
  bool virgin = hotelOfMobs.open();
  if (virgin) makeGod();
  raffleOfMsgs.open();
  return virgin;
}

void closeTank(FATE f) {
  raffleOfMsgs.close(f);
  hotelOfMobs.close(f);
  closeGlobals(f);
}

