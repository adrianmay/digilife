#include "Mob_pile/1.h"
#include "Msg_pile/1.h"
#include "Mob_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "globals/h.h"
#include "misc/h.h"
#include "h.h"

void showMobTact(MobTact t) {
  printf("Tact {%d,%x}\n", t.i.i, t.n);
}

void showTank() {
  hotelOfMobs.show();
  raffleOfMsgs.show();
}

MobTact makeGod() {
  void stuffGod(MobBody * pB) {pB->phylum=PHY_GOD;}
  Mob * p;
  hotelOfMobs.checkHotel(0);
  MobTact tChild = hotelOfMobs.admit(0, stuffGod, &p, 0);
  return tChild;
}

MobTact 
tInvestor, tSales, 
  tMemCosts, tMemLost, 
  tCpuCosts, tCpuLost, tCpuFines, 
  tUndeliverables;

void makeVirginTank() {
  tInvestor       = makeGod(); 
  tSales          = makeGod(); 
  tMemCosts       = makeGod(); 
  tMemLost        = makeGod(); 
  tCpuCosts       = makeGod(); 
  tCpuLost        = makeGod(); 
  tCpuFines       = makeGod(); 
  tUndeliverables = makeGod();
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

void choose() {raffleOfMsgs.draw();}

void onMobRentCollected(Cash cash) {
  void f(Mob * p) {hotelOfMobs.richer(p, cash);}
  hotelOfMobs.with(tMemCosts,f);
}

void onMobRentDefaulted(Cash cash) {
  void f(Mob * p) {hotelOfMobs.richer(p, cash);}
  hotelOfMobs.with(tMemLost,f);
}

void onMsgRentCollected(Cash cash) {
  void f(Mob * p) {hotelOfMobs.richer(p, cash);}
  hotelOfMobs.with(tMemCosts,f);
}

void onMsgRentDefaulted(Cash cash) {
  void f(Mob * p) {hotelOfMobs.richer(p, cash);}
  hotelOfMobs.with(tMemLost,f);
}

void burned(Core * pC, Cycles cycles, int line) {
  pC->msgCash -= cycles * pC->bid;
  if (pC->msgCash < 0) {
    longjmp(pC->jmpbuf, cycles); // Notify them in the catch after stopping job
  }
  notifyCycles(cycles);
}

bool spending(Core * pC, Cash cash, bool (*act)(void)) {
  if (cash <= pC->mobCash) { pC->mobCash -= cash; return act(); }
  else return false;
}


bool spawn(Core * pC, Cash cash, WithMobBody stuffBody, MobTact * ptChild) {
  //printf("Spawn cash=%ld\n", cash);
  bool f(void) {  
    void stuff(MobBody * pB) { if (stuffBody) stuffBody(pB); }
    *ptChild = hotelOfMobs.admit(cash, stuff, 0, 0);
    return true;
  }
  return spending(pC, cash, f);  
}

Weight bidToWeight(Cash c) { return 1; }

bool post(Core * pC, Cash cash, CpuBid bid, MobTact tR, WithPayload stuffPayload) {
  //printf("post:f: mob cash=%ld\n", pSender->rent.cash);
  if (tR.i.i == BAD_INDEX) {
    printf("post: bad rcvr index\n");
    return false;
  }
  void nowt(Mob * pMob) { }
  if (Dead == hotelOfMobs.with(tR, nowt)) {
    printf("post: dead rcvr\n");
    return false;
  }
  bool f(void) {
    void stuff(MsgTicket * pT) {
      pT->cpuBid = bid;
      pT->rcvr = tR;
      pT->sndr = pC->tMob;
      if (stuffPayload) stuffPayload(&pT->payload);
    }
    raffleOfMsgs.play(cash, bidToWeight(bid), stuff);
    return true;
  }
  return spending(pC, cash, f);
}

void mutMob(MobBody * pDst, Mob * pSrc) {
  pDst->phylum = PHY_B;
}

bool runMob(Core * pC, Mob * pMob, Msg * pMsg) {
  void stuffMob(MobBody * pMB) { mutMob(pMB, pMob); }
  void stuffPayload(MsgPayload * pP) { }
  CpuBid bid = 0;
  MobBody * pMB = &pMob->body;
  //printf("runMob start: i=%d cash=%ld\n", pC->tMob.i.i, pMob->rent.cash);
  if (pMB->phylum != PHY_B) abort();
  pC->mobCash += PAY;
  burned(pC, CYCLES_IDLE, __LINE__);
  PhyB * pB = &pMB->p.b;
  if (pC->mobCash > pB->spawnThresh) {
    burned(pC, CYCLES_EXTRA_SPAWN, __LINE__);
    Cash forChildTot = pC->mobCash/2;
    Cash forChildMob = forChildTot * (1.0 - MSG_PROP);
    Cash forChildMsg = forChildTot * MSG_PROP;
    //printf("Spawning: mycash=%ld forChildMob=%ld forChildMsg=%ld\n", pMob->rent.cash, forChildMob, forChildMsg);
    MobTact tChild;
    if (spawn(pC, forChildMob, stuffMob, &tChild))
      post(pC, forChildMsg, bid, tChild, stuffPayload);
  } 
  post(pC, pMob->rent.cash*MSG_PROP, bid, pMsg->body.ticket.rcvr, stuffPayload);
  return true;
}

void job(MobTact tMob, Mob * pMob, Msg* pMsg) {
  hotelOfMobs.collectRent(pMob, true);
  Core core;
  core.tMob = tMob;
  core.msgCash = pMsg->rent.cash;
  core.mobCash = pMob->rent.cash;
  MsgTicket * pTicket = &pMsg->body.ticket;
  core.bid = pTicket->cpuBid;
  int jmp = setjmp(core.jmpbuf);
  if (jmp == 0)  runMob(&core, pMob, pMsg);
  
  Cash change = core.mobCash + core.msgCash;
  pMob->rent.cash = change;

//  if (core.jobCash>0) hotelOfMobs.richer(pMob,   core.jobCash);
//  if (core.jobCash<0) { 
//    Cash over = -core.jobCash;
//    Cash couldAfford = hotelOfMobs.poorer(pMob, over, Ono);
//    if (couldAfford < over) {
//      void f(Mob * pMemLost) { hotelOfMobs.poorer(pMemLost, over - couldAfford, Exact); }
//      hotelOfMobs.with(tMemLost, f);
//    } else {
//      Cash fine = -core.jobCash*FINE_FACTOR;
//      hotelOfMobs.poorer(pMob, fine, Ono);
//    }
//  }
}

void onMsgRaffleDispatch(MsgIx i, Msg * pMsg, VV claim, VV unlock, VV rob) {
  MobTact tMob = pMsg->body.ticket.rcvr;
  void f(Mob * pMob) { 
    claim();
    unlock();
    job(tMob, pMob, pMsg); //
  }
  Woth woth = hotelOfMobs.with(tMob, f);
  if (woth != Busy) {
    rob();
  }

}

void onMobHotelGoDie(MobIx i, Mob * p) {}



/*
int samples=0;
Avg avg;

#define MIX(F) \
  avg.F = ( samples*avg.F + ((double)pB->p.b.F)) / ((double)(samples+1)) ;

void smple(MobBody* pB) {
  MIX(spawnThresh)
  MIX(payMsg)
  MIX(bid)
  if (samples<100) samples++;
}
*/
