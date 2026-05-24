#include <stdatomic.h>
#include <string.h>
#include "misc/h.h"
#include "args/h.h"
#include "perf/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/structs.h"
#include "Mob_hotel/h.h"
#include "h.h"

typedef struct {
  pthread_t pid;
  bool forceYield;
  Alarm alarm;
} Worker;

Worker workers[MAX_WORKER_THREADS] = {0};
Weight bidToWeight(CpuBid bid) {return bid;}

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
}
                                  
void runMob(MobIx i, Mob * p) {
  for (int x=0, a=0; a < p->body.effort; a++) 
    x+=a;
  printf("Just did Mob %d\n", i.i);
}

void * workerThread(void * p) {
  Worker * pW = (Worker *) p;
  memset(pW, 0, sizeof(Worker));
  pW->pid = pthread_self();
  initThreadAlarm(&pW->alarm, alarmHandler, pW);
  while (true) {
    MsgTicket ticket;
    Cash cash;
    if (!raffleOfMsgs.draw(&ticket, &cash)) return 0;
    Cycles cycleLimit = cash / ticket.cpuBid; //TODO: careful
    setAlarm(&pW->alarm, cycleLimit);
    runMob(ticket.iRcvr, hotelOfMobs.get(ticket.iRcvr));
    // Get output and do stuff.
  }
  //delCore(core);
  return 0;
}

void * thisWorkerThread(int w) {
  return workerThread(&workers[w]); 
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
