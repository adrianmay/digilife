#include <setjmp.h>
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

typedef struct {
  Cash jobCash;
  CpuBid bid;
  jmp_buf jmpbuf;
} Core;

void burn(Core * pC, Cycles c, int line) {
  pC->jobCash -= c * pC->bid;
  notifyCycles(c);
  //printf("burn: ")
  if (pC->jobCash < 0) {
    //printf("Burning out from line %d\n", line);
    longjmp(pC->jmpbuf, 1);
  }
}

MobTact spawn(Cash cash, MobTact tBenefactor, WithMobBody stuffBody) {
  if (cash<=0) return (MobTact) {badMobIx};
  void stuff(MobBody * pB) {
    if (stuffBody) stuffBody(pB);
  }
  MobTact tChild = (MobTact){badMobIx};
  Cash forChild;
  void f(Mob * pBene) { forChild = hotelOfMobs.poorer(pBene, cash, Exact); }
  hotelOfMobs.with(tBenefactor, f);
  if (forChild == cash) {
    Mob * pMob;
    tChild = hotelOfMobs.admit(cash, stuff, &pMob, 0);
    return tChild;
  }
  return (MobTact) {badMobIx};
}

Weight bidToWeight(CpuBid bid) {return 1;}

bool post(Cash cash, CpuBid bid, MobTact tS, MobTact tR, WithPayload stuffPayload) {
  if (tR.i.i == BAD_INDEX) abort();
  void stuff(MsgTicket * pT) {
    pT->cpuBid = bid;
    pT->rcvr = tR;
    pT->sndr = tS;
    if (stuffPayload) stuffPayload(&pT->payload);
  }
  Cash canAfford;
  void f(Mob * pMob) {
    canAfford = hotelOfMobs.poorer(pMob, cash, Ono);
  }
  hotelOfMobs.with(tS, f);
  if (canAfford>0) {
    raffleOfMsgs.play(canAfford, bidToWeight(bid), stuff);
    return true;
  } else { 
    printf("Mob %d couldn't afford %ld to post\n", tS.i.i, canAfford);
    return false;
    //abort();
  }
}

#define PAY 700
#define CYCLES_PASSIVE 10
#define CYCLES_SPAWNING  20
#define PROB_SPAWN 0.5
#define BIG 1000000
#define MSG_PROP (((double)sizeof(Msg))/((double)(sizeof(Mob)+sizeof(Msg))))

void runMob(Core * pC, Mob * pMob, Msg * pMsg) {
  MobBody * pMB = &pMob->body;
  if (pMB->phylum != PHY_B) abort();
  void pm(Mob * pSales) {
    hotelOfMobs.poorer(pSales, PAY, Exact);
    pC->jobCash += PAY;
  }
  hotelOfMobs.with(tSales, pm); 
  PhyB * pB = &pMB->p.b;
  if (randIntBelow(BIG)<BIG*PROB_SPAWN) {
    Cash forChildTot = pMob->rent.cash/2;
    Cash forChildMob = forChildTot * (1.0 - MSG_PROP);
    MobTact tChild = spawn(forChildMob, pMsg->body.ticket.rcvr, 0);
    post(forChildMob*MSG_PROP, pB->bid, pMsg->body.ticket.rcvr, tChild, 0);
  } 
  void stuffPayload(MsgPayload * p) {(void)p;}
  post(pMob->rent.cash*MSG_PROP, pB->bid, pMsg->body.ticket.rcvr, pMsg->body.ticket.rcvr, stuffPayload);

}

void job(Mob * pMob, Msg* pMsg) {
  hotelOfMobs.collectRent(pMob);
  Core core;
  core.jobCash = pMsg->rent.cash;
  MsgTicket * pTicket = &pMsg->body.ticket;
  core.bid = pTicket->cpuBid;
  int jmp = setjmp(core.jmpbuf);
  if (jmp == 0)  runMob(&core, pMob, pMsg);
  if (core.jobCash>0) hotelOfMobs.richer(pMob,   core.jobCash);
  if (core.jobCash<0) { 
    Cash over = -core.jobCash;
    Cash couldAfford = hotelOfMobs.poorer(pMob, over, Ono);
    if (couldAfford < over) {
      void f(Mob * pMemLost) { hotelOfMobs.poorer(pMemLost, over - couldAfford, Exact); }
      hotelOfMobs.with(tMemLost, f);
    } else {
      Cash fine = -core.jobCash*FINE_FACTOR;
      hotelOfMobs.poorer(pMob, fine, Ono);
    }
  }
}

void onMsgRaffleDispatch(MsgIx i, Msg * pMsg, VV claim, VV unlock, VV rob) {
  MobTact tMob = pMsg->body.ticket.rcvr;
  void f(Mob * pMob) { 
    claim();
    unlock();
    job(pMob, pMsg); //
  }
  Woth woth = hotelOfMobs.with(tMob, f);
  if (woth != Busy) {
    rob();
  }

}




void onMobHotelGoDie(MobIx i, Mob * p) {}
