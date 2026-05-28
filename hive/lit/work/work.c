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


Worker workers[MAX_WORKER_THREADS] = {0};
Weight bidToWeight(CpuBid bid) {return 1000000000*bid;}

//void onMobsExtinct(void) {
//  raffleOfMsgs.quit();
//}

void emit(Cash cash, CpuBid bid, MobIx iR, MobIx iS) {
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
  printf("Job overran in %'ld\n", pW->lastEnded - pW->lastStarted);
}
                                  
void runMob(Worker * pW, MobIx i, Mob * p) {
  if (pW->output == 0) pW->output = 1;
  if (p->body.effort == 10000001) pW->output += 10;
  if (p->body.effort == 10000002) pW->output *= 2;
  // So it's either 22 or 12 depending on the order.
  int e = p->body.effort*0.264;
  for (int x=0, a=0; a < e; a++) {
    if (pW->forceYield) return;
    x+=a;
  }
  if (pW->output == 1) pW->output=2;
  //printf("Just did Mob %d\n", i.i);
}

void * workerThread(void * p) {
  Worker * pW = (Worker *) p;
  memset(pW, 0, sizeof(Worker));
  pW->timer = initThreadTimer();
  pW->pid = pthread_self();
  initThreadAlarm(&pW->alarm, alarmHandler, pW);
  while (true) {
    raffleOfMsgs.kill();
    MsgTicket ticket;
    Cash cash;
    //raffleOfMsgs.show();
    if (!raffleOfMsgs.draw(&ticket, &cash)) {
      break;
    }
    Cycles cycleLimit = 1000 * cash / ticket.cpuBid; //TODO: careful
    printf("cycleLimit=%'ld, cash = %ld, cpuBid = %f\n", cycleLimit, cash, ticket.cpuBid);
    setAlarm(&pW->alarm, cycleLimit);
    pW->lastStarted = readThreadCycles(pW->timer);
    if (pW->firstStarted == 0) pW->firstStarted = pW->lastStarted;
    runMob(pW, ticket.iRcvr, hotelOfMobs.get(ticket.iRcvr));
    if (!pW->forceYield) {
      pW->lastEnded = readThreadCycles(pW->timer);
      printf("Job finished in %'ld - %'ld = %'ld cycles\n", pW->lastEnded, pW->lastStarted, pW->lastEnded - pW->lastStarted);
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
