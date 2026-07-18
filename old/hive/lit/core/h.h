#include "bit/MobBody.h"
#include "bit/MsgTicket.h"
#include "tank/h.h"

typedef MobTact (*Spawn)(Cash cash, WithMobBody stuffBody);
typedef MsgIx (*Post)(Cash cash, CpuBid bid, MobTact tR, WithPayload stuffPayload);
typedef struct {
  Spawn spawn;
  Post post;
} Api;

typedef struct { Cycles used; Cycles overran; } Burned;

Burned run(Api api, MobTact tMe, Cash mobCash, Cash msgCash, MobBody * pMB, MsgTicket * pTicket);

// #include "bit/MsgTicket.h"
// 
// extern FILE * outfile; // Open and close this
// 
// typedef struct Core Core;
// typedef struct Env Env;
// 
// int runCore(Core * p);
// CycleDiff runMob(Core * p, Mob * pMob, MobIx i, Env * pEnv, Cycles limit);


/*

Make a raffle of messages.
Function to make a message with cpuBid, rcvr, body

Core loop for worker threads:
  update tocks
  kill for rent
  get a message
  run with timer into action list
  do actions

*/

