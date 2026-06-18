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
  hotelOfMobs.richer(tMemCosts.i, cash);
}

void onMobRentDefaulted(Cash cash) {
  hotelOfMobs.richer(tMemLost.i, cash);
}

void onMsgRentCollected(Cash cash) {
  hotelOfMobs.richer(tMemCosts.i, cash);
}

void onMsgRentDefaulted(Cash cash) {
  hotelOfMobs.richer(tMemLost.i, cash);
}

Cash costOfCycles(CpuBid bid, Cycles cyc) {
  return cyc * bid;
}

Cash msgPayMob(MsgIx iMsg, MobTact tMob, Cash amt) {
  Cash ret = raffleOfMsgs.robUpTo(iMsg, amt);
  hotelOfMobs.richer(tMob.i, ret);
  return ret;
}

Cash mobPayMob(MobTact tFrom, MobTact tTo, Cash amt) {
  Cash ret = hotelOfMobs.poorer(tFrom.i, amt, Ono);
  hotelOfMobs.richer(tTo.i, ret);
  return ret;
}

Cash msgPayMobAll(MsgIx iMsg, MobTact tMob) {
  Cash c = raffleOfMsgs.rob(iMsg);
  hotelOfMobs.richer(tMob.i, c);
  return c;
}

Weight bidToWeight(CpuBid bid) {return 1000*bid;}

void showTank() {
  hotelOfMobs.show();
  raffleOfMsgs.show();
}

MobTact makeGod() {
  void stuffGod(MobBody * pB) {pB->phylum=PHY_GOD;}
  Mob * p;
  MobIx iChild = hotelOfMobs.admit(0, stuffGod, &p, 0);
  return (MobTact){iChild, p->body.nick};
}

MobTact spawn_(Cash cash, MobTact tBenefactor, WithMobBody stuffBody) {
  void stuff(MobBody * pB) {
    pB->nick = randIntBelow(0xFFFF);
    pB->todo = (MsgIx){BAD_INDEX};
    stuffBody(pB);
  }
  MobIx iChild = badMobIx;
  if (checkTact(tBenefactor)) {
    if (hotelOfMobs.poorer(tBenefactor.i, cash, Exact)) {
      Mob * pMob;
      iChild = hotelOfMobs.admit(cash, stuff, &pMob, 0);
      return tact(iChild);
    } else {
      return (MobTact) {badMobIx};
    }
  }
  return (MobTact) {badMobIx};
}

MobTact spawn(Cash cash, MobTact tBenefactor, WithMobBody stuffBody) {
  if (cash<=0) return (MobTact) {badMobIx};
  //showMobTact(tBenefactor);
  MobTact ret = spawn_(cash, tBenefactor, stuffBody);
  //showMobTact(ret);
  //if (ret.i.i != BAD_INDEX) 
  //  hotelOfMobs.showMob(ret.i, hotelOfMobs.get(ret.i));
  return ret;
}

MsgIx post(Cash cash, CpuBid bid, MobTact tS, MobTact tR, WithPayload stuffPayload) {
  if (tR.i.i == BAD_INDEX) abort();
  void stuff(MsgTicket * pT) {
    pT->cpuBid = bid;
    pT->rcvr = tR;
    pT->sndr = tS;
    stuffPayload(&pT->payload);
  }
  if (checkTact(tS)) {
    Cash canAfford = hotelOfMobs.poorer(tS.i, cash, Ono);
    if (canAfford>0) {
      MsgIx iMsg = raffleOfMsgs.play(canAfford, bidToWeight(bid), stuff);
      return iMsg;
    } else { 
      printf("Mob %d couldn't afford %ld to post\n", tS.i.i, canAfford);
      abort();
    }
  } else printf("Tact unreal\n");
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

bool onMsgRaffleApprove(MsgIx i, MsgTicket * pTicket) {
  MobTact tMob = pTicket->rcvr;
  Mob * pMob = derefTact(tMob);
  if (!pMob) return true; // True to have it deleted
  //printf("Approving msg %d\n", i.i); 
  //raffleOfMsgs.show();
  MsgIx exp = badMsgIx;
  bool res = atomic_compare_exchange_strong(&pMob->body.todo, &exp, i);
  return res;
}

void onMsgRaffleConsume_(MsgIx i, MsgTicket * pTicket) {
  (void)pTicket;
  Msg * pMsg = pileOfMsgs.get(i);  
  MobTact tMob = pMsg->body.ticket.rcvr;
  //showMobTact(tMob);
  //hotelOfMobs.show();
  Mob * pMob = derefTact(tMob);
  if (!pMob) return; 
  MobBody * pB = &pMob->body;
  if (pMob) {
    MsgIx todo = atomic_load(&pMob->body.todo);
    if (todo.i == i.i) {
      hotelOfMobs.collectRent(tMob.i);
      MobTact spawn_(Cash c, WithMobBody st) { 
        return spawn(c, tMob, st);
      }
      MsgIx post_(Cash c, CpuBid bid, MobTact tR, WithPayload st) { 
        return post(c, bid, tMob, tR, st); 
      } 
      Burned brnd = run( 
          (Api){spawn_, post_}, tMob, 
          pMob->rent.cash, pMsg->rent.cash, 
          pB, &pMsg->body.ticket );
      MsgIx exp = i;
      bool alive = true;
      // Try to set todo to nothing assuming it's currently i.
      // But if it isn't, that means it went backrupt for other reasons
      // but was not immediately freed cos we were using it, so now we
      // free it.
      if (!atomic_compare_exchange_strong(&pMob->body.todo, &exp, badMsgIx)) {
        if (exp.i != BAD_INDEX-1) abort();  
        pileOfMobs.free(tMob.i);
        alive = false; //If the mob is financed now and proceeds to the below, another thread
                       //could rob the mob and crash the code below
      }
      Cash costUsed = costOfCycles(pMsg->body.ticket.cpuBid, brnd.used);
      if (alive) {
        if (brnd.overran <= 0) {
          msgPayMob(i, tCpuCosts, costUsed); // Pay for cpu used
          msgPayMobAll(i, tMob); // Change goes to rcvr
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
}

void onMsgRaffleConsume(MsgIx i, MsgTicket * pTicket) {
  onMsgRaffleConsume_(i, pTicket);
  raffleOfMsgs.cancel(i);
}

bool onMobHotelGoDie(MobIx i, Mob * pMob) {
  MsgIx todo = atomic_exchange(&pMob->body.todo, (MsgIx){BAD_INDEX-1});
  bool res = todo.i == BAD_INDEX;
  if (res) pMob->body.nick = BAD_INDEX;
  return res;
}

void choose() {raffleOfMsgs.draw();}


