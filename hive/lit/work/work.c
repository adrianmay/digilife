#include <stdatomic.h>
#include "perf/h.h"
#include "misc/h.h"
#include "args/h.h"
#include "lang/h.h"
#include "Msg_raffle/h.h"
#include "h.h"

Weight bidToWeight(CpuBid bid) {return bid;}

void emit(Cash cash, CpuBid bid, MsgTicket * pTicket) {
  raffleOfMsgs.enter(cash, bidToWeight(bid), pTicket);
}

void runUnlimited(MsgTicket * m) { 
}

                                  //
void * work(void * p) {
  (void)(p);
  CoreHandle core = newCore();
  while (!quitting(core)) {
    MsgTicket ticket;
    Cash cash;
    if (!raffleOfMsgs.draw(&ticket, &cash)) break;
    //runLimited();
  }
  delCore(core);
  return 0;
}

// Assume files open
void initWork() { 
  //Start the workers
  for (int w=0;w<getNumWorkers();w++) {
    pthread_t pid;
    pthread_create(&pid, 0, work, 0);
  }
}

