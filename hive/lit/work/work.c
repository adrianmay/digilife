#include <stdatomic.h>
#include "misc/h.h"
#include "args/h.h"
#include "core/h.h"
#include "Msg_raffle/h.h"
#include "Mob_hotel/h.h"
#include "h.h"

Weight bidToWeight(CpuBid bid) {return bid;}

void emit(Cash cash, CpuBid bid, MsgTicket * pTicket) {
  raffleOfMsgs.enter(cash, bidToWeight(bid), pTicket);
}

void runUnlimited(MsgTicket * m) { 
}

static void alarmHandler(int core) {
  forceYield(core);
}
                                  
void * workerThread(void * p) {
  (void)(p);
  CoreHandle core = newCore();
  initThreadAlarm(coreAlarm(core), alarmHandler, core);
  while (!quitting(core)) {
    MsgTicket ticket;
    Cash cash;
    if (!raffleOfMsgs.draw(&ticket, &cash)) break;
    Cycles cycleLimit = cash / ticket.cpuBid; //TODO: careful
    setAlarm(coreAlarm(core), cycleLimit);
    runMob(hotelOfMobs.get(ticket.iRcvr));
    // Get output and do stuff.
  }
  delCore(core);
  return 0;
}

// Assume files open
void initWork() { 
  //Start the workers
  for (int w=0;w<getNumWorkers();w++) {
    pthread_t pid;
    pthread_create(&pid, 0, workerThread, 0);
  }
}

