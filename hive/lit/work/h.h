
#include "bit/MsgTicket.h"

typedef struct {
  pthread_t pid;
  bool forceYield;
  Timer timer;
  Alarm alarm;
  int output;
  Cycles firstStarted;
  Cycles lastStarted;
  Cycles lastEnded;
} Worker;

Worker * thisWorker(int w);
int runWorker(Worker * pW);
void emit(Cash cash, CpuBid bid, MobIx iRcvr, Ix nRcvr, MobIx iSndr);
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

