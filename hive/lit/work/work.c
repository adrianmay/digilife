#include <stdatomic.h>
#include "perf/h.h"
#include "misc/h.h"
#include "args/h.h"
#include "Msg_raffle/h.h"
#include "h.h"

Weight bidToWeight(CpuBid bid) {return bid;}

void emit(Cash cash, CpuBid bid, MsgTicket * pTicket) {
  raffleOfMsgs.enter(cash, bidToWeight(bid), pTicket);
}

void runUnlimited(MsgTicket * m) { 
}

void work_(void) {
  Cash cash;
  MsgTicket ticket;
  if (!raffleOfMsgs.draw(&ticket, &cash)) return;
  
}

atomic_int numWorkersRunning = 0; // Can read with int x = atomic_load(&numWorkersRunning);
                                  //
void * work(void * p) {
  (void)(p);
  atomic_fetch_add(&numWorkersRunning, 1);
  work_();
  atomic_fetch_sub(&numWorkersRunning, 1);
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

