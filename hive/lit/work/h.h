
#include "bit/MsgTicket.h"

void * thisWorkerThread(int w);
void emit(Cash cash, CpuBid bid, MobIx iRcvr, MobIx iSndr);
void initWork();
void waitWorkersAllDone();

/*

Make a raffle of messages.
Function to make a message with cpuBid, rcvr, body

Worker loop for worker threads:
  update tocks
  kill for rent
  get a message
  run with timer into action list
  do actions

*/ 

