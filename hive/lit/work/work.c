#include <stdatomic.h>
#include <string.h>
#include <unistd.h>
#include "misc/h.h"
#include "args/h.h"
#include "perf/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/structs.h"
#include "Mob_hotel/h.h"
#include "h.h"

#define MUT_RATE 0.25

Worker workers[MAX_WORKER_THREADS] = {0};
Weight bidToWeight(CpuBid bid) {return 1000000000*bid;}

//void onMobsExtinct(void) { onXXsExtinct
//  raffleOfMsgs.quit();
//}

void emit(Cash cash, CpuBid bid, MobIx iR, MobIx iS) {
  //if (cash>10000) { printf("Overrich 3 %d has %'ld\n", iR.i, cash); abort(); }
  static MsgTicket tmp;
  tmp.cpuBid = bid;
  tmp.iRcvr = iR;
  if (hotelOfMobs.chargeIfCan(iS, cash))
    raffleOfMsgs.enter(cash, bidToWeight(bid), &tmp);
}

static void alarmHandler(void * p) {
  Worker * pW = (Worker *) p;
  pW->forceYield = true;
  setAlarm(&pW->alarm, 0);
  pW->lastEnded = readThreadCycles(pW->timer);
  //printf("Job overran in %'ld\n", pW->lastEnded - pW->lastStarted);
}

void runMobA(Worker * pW, MobIx i, Mob * p) {
  if (pW->output == 0) pW->output = 1;
  if (p->body.p.a.effort == 10000001) pW->output += 10;
  if (p->body.p.a.effort == 10000002) pW->output *= 2;
  // So it's either 22 or 12 depending on the order.
  int e = p->body.p.a.effort*0.264;
  for (int x=0, a=0; a < e; a++) {
    if (pW->forceYield) return;
    x+=a;
  }
  if (pW->output == 1) pW->output=2;
  //printf("Just did Mob %d\n", i.i);
}


bool rollProb(double p) {
  uint64_t m = 65536;
  uint64_t t = m*p;
  uint64_t r = randIntBelow(m);
  return (r<t);
}

double mutProb(double p) {
  double g = gaussian_random(0, 0.5*MUT_RATE);
  return p+g;
}

void spawnB(MobIx iParent, Mob * pParent) {
  //printf("Hoping to spawn... ");
  //hotelOfMobs.showMob(iParent, pParent);
  PhyB * pPB = &pParent->body.p.b;
  Mob * pChild;
  Cash forChildMsg = pPB->spawnCash * pPB->fractionMsgOfMobSize; 
  Cash mustHave = pPB->spawnCash + forChildMsg; // Processing cost too
  //printf("Must have %ld\n", mustHave);
  if (pParent->rent.cash < mustHave) 
    return;
  //printf("Spawning...\n");
  MobIx iChild = hotelOfMobs.alloc(pPB->spawnCash, &pChild, 0);
  hotelOfMobs.enrich(iParent, -pPB->spawnCash);
  PhyB * pCB = &pChild->body.p.b;
  pChild->body.phylum = pParent->body.phylum;
  pCB->spawnCash = pPB->spawnCash;
  pCB->pay = pPB->pay;
  pCB->bid = pPB->bid;
  pCB->fractionMsgOfMobSize = pPB->fractionMsgOfMobSize;
  pCB->oddsLazyByCash0 = mutProb(pPB->oddsLazyByCash0);
  pCB->oddsLazyByCash1 = mutProb(pPB->oddsLazyByCash1);
  pCB->msgCashByCash0  = mutProb(pPB->msgCashByCash0);
  pCB->msgCashByCash1  = mutProb(pPB->msgCashByCash1);
  emit(forChildMsg, pCB->bid, iChild, iParent);
  hotelOfMobs.review(iParent);
  hotelOfMobs.kill();
}

int iter=0;
void runMobB(Worker * pW, MobIx i, Mob * pMob) {
  printf("%d %d %d\n", iter++, hotelOfMobs.count(), raffleOfMsgs.count());
  PhyB * p = &pMob->body.p.b;
  //if (pMob->rent.cash>10000) abort();
  hotelOfMobs.enrich(i, p->pay);
  //if (pMob->rent.cash>10000) abort();
  emit(pMob->rent.cash * p->fractionMsgOfMobSize, p->bid, i, i);
  //if (pMob->rent.cash>10000) abort();
  // Poorer now
  //double pYield = clampProb(p->oddsLazyByCash0 - 0.5 + p->oddsLazyByCash1 * pMob->rent.cash/2.0/p->spawnCash);
  //printf("pYield=%f\n", pYield);
  //if (rollProb(pYield)) return;
  if (rollProb(0.8)) return;
  spawnB(i, pMob);
}

void runMob(Worker * pW, MobIx i, Mob * p) {
  hotelOfMobs.review(i);
  hotelOfMobs.kill();
  switch (p->body.phylum) {
    case 'a':
      runMobA(pW, i, p);
      break;
    case 'b':
      runMobB(pW, i, p);
      break;
    default:
      printf("Unknown phylum: %c\n", p->body.phylum);
      exit(1);
  }
}

void * workerThread(void * p) {
  Worker * pW = (Worker *) p;
  memset(pW, 0, sizeof(Worker));
  pW->timer = initThreadTimer();
  pW->pid = pthread_self();
  initThreadAlarm(&pW->alarm, alarmHandler, pW);
  while (iter<60000) {
    raffleOfMsgs.kill();
    MsgTicket ticket;
    Cash cash;
    //raffleOfMsgs.show();
    if (!raffleOfMsgs.draw(&ticket, &cash)) {
      break;
    }
    //if (cash>10000) { printf("Overrich 4 %d has %'ld\n", ticket.iRcvr.i, cash); abort(); }
    Cycles cycleLimit = 1000 * cash / ticket.cpuBid; //TODO: careful
    //printf("cycleLimit=%'ld, cash = %ld, cpuBid = %f\n", cycleLimit, cash, ticket.cpuBid);
    setAlarm(&pW->alarm, cycleLimit);
    pW->lastStarted = readThreadCycles(pW->timer);
    if (pW->firstStarted == 0) pW->firstStarted = pW->lastStarted;
    runMob(pW, ticket.iRcvr, hotelOfMobs.get(ticket.iRcvr));
    if (!pW->forceYield) {
      pW->lastEnded = readThreadCycles(pW->timer);
      CycleDiff used = pW->lastEnded - pW->lastStarted;
      //if (cash>10000) { printf("Overrich 5 %d has %'ld\n", ticket.iRcvr.i, cash); abort(); }
      cash -= used * ticket.cpuBid / 1000;
      ////if (cash>10000) { printf("Overrich 6 %d has %'ld; used=%'ld, lastEnded=%'ld, lastStarted=%'ld\n", ticket.iRcvr.i, cash, used, pW->lastEnded, pW->lastStarted); abort(); }
      //printf("Job finished in %'ld - %'ld = %'ld cycles with change = %'ld\n", pW->lastEnded, pW->lastStarted, used, cash);
      hotelOfMobs.enrich(ticket.iRcvr, cash);
    }
    setAlarm(&pW->alarm, 0);
    // Get output and do stuff.
  }
  unitThreadAlarm(&pW->alarm);
  unitThreadTimer(pW->timer);
  return 0;
}

Worker * thisWorker(int w) {
  return &workers[w];
}

int runWorker(Worker * pW) {
  workerThread(pW);
  return pW->output;
}
// Assume files open
void initWork() {
  //Start the workers
  for (int w=0;w<getNumWorkers();w++) {
    pthread_t junk;
    pthread_create(&junk, 0, workerThread, &workers[w]);
  }
}

void waitWorkersAllDone() {
  for (int w=0;w<getNumWorkers();w++) {
    if (workers[w].pid)
      pthread_join(workers[w].pid,0);
  }
}
