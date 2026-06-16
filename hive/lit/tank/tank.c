#include "Mob_pile/1.h"
#include "Msg_pile/1.h"
#include "Mob_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "globals/h.h"
#include "misc/h.h"
#include "core/h.h"
#include "h.h"

void showMobTact(MobTact t) {
  printf("Tact {%d,%x}\n", t.i.i, t.n);
}

MobTact tact(MobIx i) {
  Mob * pMob = hotelOfMobs.get(i);
  Nick nick = pMob->body.nick;
  return (MobTact) {i, nick};
}

bool checkTact_(MobTact t) {
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

bool checkTact(MobTact t) {
  if (!checkTact_(t)) abort();
  return true;
}

Mob * derefTact(MobTact t) {
  Mob * p = hotelOfMobs.get(t.i);
  if (p->body.nick == t.n)
    return p;
  else 
    return 0;
}

MobTact 
  tInvestor, tSales, 
  tMemCosts, tMemLost, 
  tCpuCosts, tCpuLost, tCpuFines, 
  tUndeliverables;

void onMobRentCollected(Cash cash) {
  hotelOfMobs.enrich(tMemCosts.i, cash);
}

void onMobRentDefaulted(Cash cash) {
  hotelOfMobs.enrich(tMemLost.i, cash);
}

void onMsgRentCollected(Cash cash) {
  hotelOfMobs.enrich(tMemCosts.i, cash);
}

void onMsgRentDefaulted(Cash cash) {
  hotelOfMobs.enrich(tMemLost.i, cash);
}

Cash costOfCycles(CpuBid bid, Cycles cyc) {
  return cyc * bid;
}

bool msgPayMob(MsgIx iMsg, MobTact tMob, Cash amt) {
  raffleOfMsgs.enrich(iMsg, -amt);
  hotelOfMobs.enrich(tMob.i, amt);
  return true;
}

Cash mobPayMob(MobTact tFrom, MobTact tTo, Cash amt) {
  Cash ret = hotelOfMobs.robUpTo(tFrom.i, amt);
  hotelOfMobs.enrich(tTo.i, ret);
  return ret;
}

Cash msgPayMobAll(MsgIx iMsg, MobTact tMob) {
  Cash c = raffleOfMsgs.rob(iMsg);
  hotelOfMobs.enrich(tMob.i, c);
  return c;
}

Weight bidToWeight(CpuBid bid) {return 10000000*bid;}

void showTank() {
  hotelOfMobs.show();
  raffleOfMsgs.show();
}

MobTact makeGod() {
  Mob * p;
  MobIx iChild = hotelOfMobs.admit(0, 0, &p, 0);
  return (MobTact){iChild, p->body.nick};
}

MobTact spawn(Cash cash, MobTact tBenefactor, WithMobBody stuffBody) {
  void stuff(MobBody * pB) {
    pB->nick = randIntBelow(0xFFFF);
    pB->todo = (MsgIx){BAD_INDEX};
    stuffBody(pB);
  }
  MobIx iChild = badMobIx;
  if (checkTact(tBenefactor)) {
    if (hotelOfMobs.chargeIfCan(tBenefactor.i, cash)) {
      Mob * pMob;
      iChild = hotelOfMobs.admit(cash, stuff, &pMob, 0);
      return tact(iChild);
    } else {
      return (MobTact) {badMobIx};
    }
  }
  return (MobTact) {badMobIx};
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

// DeliverResult deliver(Runner runner, Msg * pMsg) {
//   MobTact tMob = pMsg->body.ticket.rcvr;
//   Mob * pMob = derefTact(tMob);
//   if (!pMob) { 
//     // TODO: Pay to intestate msg death account
//     return (DeliverResult) {false}; 
//   }
//   CostAndResult car = runner(pMob);
//   if (car.cost>0) { // Not overworked so give change to rcvr
//     hotelOfMobs.enrich(tMob.i, pMsg->rent.cash - car.cost);
//   }
//   return (DeliverResult){true, car.res};
// }

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

//static bool shouldQuit = false;
//void onMobsExtinct() { shouldQuit = true; }
//void onMsgsExtinct() {}


bool onChosen(MsgIx i, MsgTicket * pTicket) {
  MobTact tMob = pTicket->rcvr;
  Mob * pMob = derefTact(tMob);
  MsgIx exp = badMsgIx;
  bool res = atomic_compare_exchange_strong(&pMob->body.todo, &exp, i);
  return res;
}

extern void onMsgRaffleGoDie(MsgIx i) { 
  Msg * pMsg = pileOfMsgs.get(i);  
  MobTact tMob = pMsg->body.ticket.rcvr;
  Mob * pMob = derefTact(tMob);
  MobBody * pB = &pMob->body;
  if (pMob) {
    MsgIx todo = atomic_load(&pMob->body.todo);
    if (todo.i == i.i) {
      hotelOfMobs.collectRent(tMob.i);
      MobTact spawn_(Cash c, WithMobBody st) { return spawn(c, tMob, st); }
      MsgIx post_(Cash c, CpuBid bid, MobTact tR, WithPayload st) { return post(c, bid, tMob, tR, st); }
      Burned brnd = run( 
          (Api){spawn_, post_}, tMob, 
          pMob->rent.cash, pMsg->rent.cash, 
          pB, &pMsg->body.ticket );
      MsgIx exp = i;
      bool alive = true;
      if (!atomic_compare_exchange_strong(&pMob->body.todo, &exp, badMsgIx)) {
        if (exp.i != BAD_INDEX-1) abort();  
        pileOfMobs.free(tMob.i);
        alive = false;
      }
      Cash costUsed  = costOfCycles(pMsg->body.ticket.cpuBid, brnd.used);
      if (alive) {
        if (brnd.overran <= 0) {
          msgPayMob(i, tCpuCosts, costUsed); // Pay for cpu used
          msgPayMobAll(i, tMob); // Change goes to rcvr
          // Give change to mob
        } else {
          Cash c = msgPayMobAll(i, tCpuCosts); // Msg pays what it can
          Cash unpaidByMsg = costUsed - c;
          Cash unpaid = unpaidByMsg - mobPayMob(tMob, tCpuCosts, unpaidByMsg); // Mob pays what it can
          mobPayMob(tCpuLost, tCpuCosts, unpaid); // Losses account pays the rest
          mobPayMob(tMob, tCpuFines, unpaidByMsg*FINE_FACTOR); //Potentially lethal
        }
      } else { // Mob is dead
        if (brnd.overran <= 0) {
          msgPayMob(i, tCpuCosts, costUsed); // Pay for cpu used
          msgPayMobAll(i, tUndeliverables); // Died intestate - king gets the spoils
        } else {
          Cash c = msgPayMobAll(i, tCpuCosts); // Msg pays what it can
          Cash unpaid = costUsed - c;
          mobPayMob(tCpuLost, tCpuCosts, unpaid); // Losses account pays the rest
        }
      }
    } 
  }
  pileOfMsgs.free(i);
}

void onMobHotelGoDie(MobIx i) {
  Mob * pMob = pileOfMobs.get(i);
  MsgIx todo = atomic_exchange(&pMob->body.todo, (MsgIx){BAD_INDEX-1});
  if (todo.i == BAD_INDEX) 
    pileOfMobs.free(i);
}

void choose() {raffleOfMsgs.draw(onChosen);}


