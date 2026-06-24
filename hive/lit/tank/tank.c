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
  MobTact tMob;
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

MobTact spawn(Cash cash, Mob * pBene, MobTact tBenefactor, WithMobBody stuffBody) {
  //printf("Spawn cash=%ld\n", cash);
  if (cash<=0) {
    printf("Can't spawn with no cash\n");
    return (MobTact) {badMobIx};
  }
  void stuff(MobBody * pB) {
    if (stuffBody) stuffBody(pB);
  }
  MobTact tChild = (MobTact){badMobIx};
  Cash forChild;
  forChild = hotelOfMobs.poorer(pBene, cash, Exact); 
  if (forChild == cash) {
    Mob * pMob;
    tChild = hotelOfMobs.admit(cash, stuff, &pMob, 0);
    return tChild;
  }
  return (MobTact) {badMobIx};
}

Weight bidToWeight(CpuBid bid) {return 1;}

bool post(Cash cash, CpuBid bid, Mob * pSender, MobTact tS, MobTact tR, WithPayload stuffPayload) {
  //printf("post:f: mob cash=%ld\n", pSender->rent.cash);
  if (tR.i.i == BAD_INDEX) {
    printf("post: bad rcvr index\n");
    return false;
  }
  void stuff(MsgTicket * pT) {
    pT->cpuBid = bid;
    pT->rcvr = tR;
    pT->sndr = tS;
    if (stuffPayload) stuffPayload(&pT->payload);
  }
  Cash canAfford=0;
  canAfford = hotelOfMobs.poorer(pSender, cash, Ono);
  if (canAfford>0) {
    raffleOfMsgs.play(canAfford, bidToWeight(bid), stuff);
    return true;
  } else { 
    printf("Mob %d couldn't afford %ld to post\n", tS.i.i, canAfford);
    return false;
    //abort();
  }
}
#define TARGET_POP 1000
#define CPU_BID 0
#define IDLE_COST 1
#define SPAWN_EXTRA_COST 1
#define SPAWN_TOT_COST (IDLE_COST+SPAWN_EXTRA_COST)
#define SPAWN_THRESH (TARGET_POP*2*SPAWN_TOT_COST)

#define SIZE_MOB ((double)sizeof(Mob))
#define SIZE_MSG ((double)sizeof(Msg))
#define SIZE_BOTH (SIZE_MOB+SIZE_MSG)
#define MSG_PROP (SIZE_MSG/SIZE_BOTH)
#define RENT_PER_TOCK (SIZE_BOTH*GUESS_GROATS_PER_TOCK_PER_BYTE)
#define RENT_PER_CYCLE (RENT_PER_TOCK/GUESS_CYCLES_PER_TOCK)

#define CYCLES_IDLE (IDLE_COST/RENT_PER_CYCLE) 
#define CYCLES_EXTRA_SPAWN (SPAWN_EXTRA_COST/RENT_PER_CYCLE) 

#define PAY 1000
#define BIG 1000000

static void train(MobBody * pMB) {
 pMB->phylum = PHY_B;
}

void runMob(Core * pC, Mob * pMob, Msg * pMsg) {
  CpuBid bid = 0;
  MobBody * pMB = &pMob->body;
  //printf("runMob start: i=%d cash=%ld\n", pC->tMob.i.i, pMob->rent.cash);
  if (pMB->phylum != PHY_B) abort();
  void pm(Mob * pSales) {
    hotelOfMobs.poorer(pSales, PAY, Exact);
    pC->jobCash += PAY;
  }
  hotelOfMobs.with(tSales, pm); 
  notifyCycles(CYCLES_IDLE);
  PhyB * pB = &pMB->p.b;
  void stuffPayload(MsgPayload * p) {(void)p;}
  if (pMob->rent.cash>pB->spawnThresh) {
    notifyCycles(CYCLES_EXTRA_SPAWN);
    Cash forChildTot = pMob->rent.cash/2;
    Cash forChildMob = forChildTot * (1.0 - MSG_PROP);
    Cash forChildMsg = forChildTot * MSG_PROP;
    //printf("Spawning: mycash=%ld forChildMob=%ld forChildMsg=%ld\n", pMob->rent.cash, forChildMob, forChildMsg);
    MobTact tChild = spawn(forChildMob, pMob, pMsg->body.ticket.rcvr, train);
    post(forChildMsg, bid, pMob, pMsg->body.ticket.rcvr, tChild, stuffPayload);
  } 
  post(pMob->rent.cash*MSG_PROP, bid, pMob, pMsg->body.ticket.rcvr, pMsg->body.ticket.rcvr, stuffPayload);

}

void job(MobTact tMob, Mob * pMob, Msg* pMsg) {
  hotelOfMobs.collectRent(pMob, true);
  Core core;
  core.tMob = tMob;
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
    job(tMob, pMob, pMsg); //
  }
  Woth woth = hotelOfMobs.with(tMob, f);
  if (woth != Busy) {
    rob();
  }

}




void onMobHotelGoDie(MobIx i, Mob * p) {}
