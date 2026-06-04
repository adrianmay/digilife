#include <stdatomic.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "misc/h.h"
#include "args/h.h"
#include "perf/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/structs.h"
#include "Mob_hotel/h.h"
#include "h.h"

#define ITERS 100000000.0
#define START_MUT_RATE 2.0
#define MUT_RATE (START_MUT_RATE - ((double)iter) / ITERS )

int iter=0;
FILE * outfile;

Worker workers[MAX_WORKER_THREADS] = {0};
Weight bidToWeight(CpuBid bid) {return 1000000000*bid;}

//void onMobsExtinct(void) { onXXsExtinct
//  raffleOfMsgs.quit();
//}

void emit(Cash cash, CpuBid bid, MobIx iR, Ix nRcvr, MobIx iS) {
  //if (cash>10000) { printf("Overrich 3 %d has %'ld\n", iR.i, cash); abort(); }
  static MsgTicket tmp;
  tmp.cpuBid = bid;
  tmp.iRcvr = iR;
  tmp.nRcvr = nRcvr;
  hotelOfMobs.review(iS);
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

double mutF(double p, double sd) {
  double g = gaussian_random(0, sd);
  return p+g;
}

double mutScale(double p, double sd) {
  double g = gaussian_random(0, sd*p);
  return p+g;
}

int mutI(int i, double sd) {
  return round(mutF( (double)i, sd ));
}

void spawnB(MobIx iParent, Mob * pParent) {
  //printf("Hoping to spawn... ");
  //hotelOfMobs.showMob(iParent, pParent);
  PhyB * pPB = &pParent->body.p.b;
  Mob * pChild;
  Cycles jobCycles = 1000000;
  Cash jobCash = pPB->bid*jobCycles;
  Cash nowCash = pParent->rent.cash;
  Cash restCash = nowCash - jobCash;
  if (restCash < 0) return;
  Cash totChildCash = restCash/2;
  Cash msgCash = totChildCash * pPB->fractionMsgOfMobSize / (1 + pPB->fractionMsgOfMobSize);
  Cash childCash = totChildCash - msgCash;
  //printf("Spawning...\n");
  MobIx iChild = hotelOfMobs.alloc(childCash, &pChild, 0);
  hotelOfMobs.enrich(iParent, childCash);
  PhyB * pCB = &pChild->body.p.b;
  pChild->body.phylum = pParent->body.phylum;
  pCB->pay = pPB->pay;
  pCB->bid = pPB->bid;
  pCB->fractionMsgOfMobSize = pPB->fractionMsgOfMobSize;
  pCB->spawnThresh = mutI(pPB->spawnThresh,MUT_RATE);
  emit(msgCash, pCB->bid, iChild, pChild->rent.nick, iParent);
  hotelOfMobs.review(iParent);
  hotelOfMobs.kill();
}

static void burn(int effort) {
  int x=0;
  for (int a=0;a<1000*effort;a++) x+=a;
}

void runMobB(Worker * pW, MobIx i, Mob * pMob) {
  if (randIntBelow(100000)<5) {
    hotelOfMobs.rob(i);
    return;
  }
  PhyB * p = &pMob->body.p.b;
  Cash bestCash = pMob->rent.cash;
  hotelOfMobs.enrich(i, p->pay - 0.1*bestCash);
  bestCash = pMob->rent.cash;
  bool spawn = bestCash > p->spawnThresh;
  if (spawn) {
    spawnB(i, pMob);
    burn(300); //Spawn+selfmsg cost = 4*selfmsg alone
  }
  burn(144);
  emit(pMob->rent.cash * p->fractionMsgOfMobSize, p->bid, i, pMob->rent.nick, i);
  //if (0==iter%10) 
    fprintf(outfile, "%d %d %d %d %ld %b\n"
                  , iter, hotelOfMobs.count(), raffleOfMsgs.count(), p->spawnThresh, bestCash, spawn);
  iter++;
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
  outfile = fopen("out","w");
  Worker * pW = (Worker *) p;
  memset(pW, 0, sizeof(Worker));
  pW->timer = initThreadTimer();
  pW->pid = pthread_self();
  initThreadAlarm(&pW->alarm, alarmHandler, pW);
  while (iter<ITERS) {
    raffleOfMsgs.kill();
    MsgTicket ticket;
    Cash cash;
    //raffleOfMsgs.show();
    if (!raffleOfMsgs.draw(&ticket, &cash)) {
      break;
    }
    //if (cash>10000) { printf("Overrich 4 %d has %'ld\n", ticket.iRcvr.i, cash); abort(); }
    Cycles cycleLimit = cash / ticket.cpuBid; //TODO: careful
    //printf("cycleLimit=%'ld, cash = %ld, cpuBid = %f\n", cycleLimit, cash, ticket.cpuBid);
    setAlarm(&pW->alarm, cycleLimit);
    pW->lastStarted = readThreadCycles(pW->timer);
    if (pW->firstStarted == 0) pW->firstStarted = pW->lastStarted;
    Mob * pMob = hotelOfMobs.get(ticket.iRcvr);
    if (pMob->rent.nick & 0x80000000 || pMob->rent.nick != ticket.nRcvr) 
      continue;
    runMob(pW, ticket.iRcvr, pMob);
    if (pW->forceYield) {
      pW->forceYield = false;
      //printf("FORCEYIELD\n");
      hotelOfMobs.enrich(ticket.iRcvr, -25);
    } else {
      hotelOfMobs.enrich(ticket.iRcvr, 25);
      pW->lastEnded = readThreadCycles(pW->timer);
      CycleDiff used = pW->lastEnded - pW->lastStarted;
      //if (cash>10000) { printf("Overrich 5 %d has %'ld\n", ticket.iRcvr.i, cash); abort(); }
      cash -= used * ticket.cpuBid;
      ////if (cash>10000) { printf("Overrich 6 %d has %'ld; used=%'ld, lastEnded=%'ld, lastStarted=%'ld\n", ticket.iRcvr.i, cash, used, pW->lastEnded, pW->lastStarted); abort(); }
      hotelOfMobs.enrich(ticket.iRcvr, cash);
      cash = hotelOfMobs.get(ticket.iRcvr)->rent.cash;
      //printf("\tTook: %'ld - %'ld = %'ld cycles with change = %'ld\n", pW->lastEnded, pW->lastStarted, used, cash);
    }
    setAlarm(&pW->alarm, 0);
    // Get output and do stuff.
  }
  unitThreadAlarm(&pW->alarm);
  unitThreadTimer(pW->timer);
  fclose(outfile);
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
