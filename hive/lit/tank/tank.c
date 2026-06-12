#include "Mob_pile/1.h"
#include "Msg_pile/1.h"
#include "Mob_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "globals/h.h"
#include "misc/h.h"
#include "h.h"

extern void onMsgRaffleGoDie(MsgIx i) { 
  Msg * pMsg = pileOfMsgs.get(i);  
}

void onMobHotelGoDie(MobIx i) {
  pileOfMobs.free(i);
}

void onMobRentCollected(Cash cash) {
  hotelOfMobs.enrich(tCostsMem.i, cash);
}

void onMobRentDefaulted(Cash cash) {
  hotelOfMobs.enrich(tRentDefaults.i, cash);
}

void onMsgRentCollected(Cash cash) {
  hotelOfMobs.enrich(tCostsMem.i, cash);
}

void onMsgRentDefaulted(Cash cash) {
  hotelOfMobs.enrich(tRentDefaults.i, cash);
}

Weight bidToWeight(CpuBid bid) {return 10000000*bid;}

void showMobTact(MobTact t) {
  printf("Tact {%d,%x}\n", t.i.i, t.n);
}

void showTank() {
  hotelOfMobs.show();
  raffleOfMsgs.show();
}

MobTact tact(MobIx i) {
  Mob * pMob = hotelOfMobs.get(i);
  Nick nick = pMob->body.nick;
  return (MobTact) {i, nick};
}

bool checkTact(MobTact t) {
  if (t.i.i == BAD_INDEX)  {
    showMobTact(t);
    return false;
  }
  Mob * p = hotelOfMobs.get(t.i);
  bool res = t.n == p->body.nick;
  if (!res) {
    printf("Tact doesn't match mob's nick: %d\n", p->body.nick);
    showMobTact(t);
  }
  return res;
}

void checkTactAbort(MobTact t) {
  if (!checkTact(t)) abort();
}

Mob * derefTact(MobTact t) {
  Mob * p = hotelOfMobs.get(t.i);
  if (p->body.nick == t.n)
    return p;
  else 
    return 0;
}

MobTact 
  tInvestor, tSales, tCostsCpu, tCostsMem,
  tRentDefaults, tUndeliverables;

MobTact makeGod() {
  Mob * p;
  MobIx iChild = hotelOfMobs.admit(0, 0, &p, 0);
  return (MobTact){iChild, p->body.nick};
}

MobTact spawn(Cash cash, MobTact tBenefactor, WithMobBody stuffBody) {
  void stuff(MobBody * pB) {
    pB->nick = randIntBelow(0xFFFF);
    stuffBody(pB);
  }
  MobIx iChild = badMobIx;
  if (checkTact(tBenefactor) && hotelOfMobs.chargeIfCan(tBenefactor.i, cash)) {
    Mob * pMob;
    iChild = hotelOfMobs.admit(cash, stuff, &pMob, 0);
    return tact(iChild);
  } else {
    return (MobTact) {badMobIx};
  }
}

MsgIx post(Cash cash, CpuBid bid, MobTact tS, MobTact tR, WithPayload stuffPayload) {
  void stuff(MsgTicket * pT) {
    pT->cpuBid = bid;
    pT->rcvr = tR;
    pT->sndr = tS;
    stuffPayload(&pT->payload);
  }
  if (checkTact(tS) && hotelOfMobs.chargeIfCan(tS.i, cash)) {
    MsgIx iMsg = raffleOfMsgs.play(cash, bidToWeight(bid), stuff);
    return iMsg;
  }
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

void makeVirginTank() {
  tInvestor = makeGod();
  tSales    = makeGod();
  tCostsCpu = makeGod();
  tCostsMem = makeGod();
}

bool openTank() {
  openGlobals();
  bool virgin = hotelOfMobs.open();
  if (virgin) makeVirginTank();
  raffleOfMsgs.open();
  return virgin;
}

void closeTank(FATE f) {
  raffleOfMsgs.close(f);
  hotelOfMobs.close(f);
  closeGlobals(f);
}

void onMobsExtinct() {}
void onMsgsExtinct() {}

void workerThread() {
  
}
