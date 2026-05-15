
#include "bit/MsgTicket.h"

void initWork();
void emit(Cash cash, CpuBid bid, MobIx rcvr, MsgTicket body);

/*

Make a raffle of messages.
Function to make a message with cpubid, rcvr, body

Worker loop for worker threads:
  update tocks
  kill for rent
  get a message
  run with timer into action list
  do actions

*/ 

