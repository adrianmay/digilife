#pragma once

#include <setjmp.h>
#include "Msg_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "bit/MsgTicket.h"

//typedef WithXXBody WithXXBody;

#define TARGET_POP 1000
#define CPU_BID 0
#define IDLE_COST 1
#define SPAWN_EXTRA_COST 1
#define SPAWN_TOT_COST (IDLE_COST+SPAWN_EXTRA_COST)
#define SPAWN_THRESH (TARGET_POP*2*SPAWN_TOT_COST)

#define SIZE_MOB ((double)sizeof(Mob))
#define SIZE_MSG ((double)sizeof(Msg))
#define SIZE_BOTH (SIZE_MOB+SIZE_MSG)
#define MSG_PROP (SIZE_MSG/SIZE_BOTH)
#define RENT_PER_TOCK (SIZE_BOTH*GUESS_GROATS_PER_TOCK_PER_BYTE)
#define RENT_PER_CYCLE (RENT_PER_TOCK/GUESS_CYCLES_PER_TOCK)

#define CYCLES_IDLE (IDLE_COST/RENT_PER_CYCLE) 
#define CYCLES_EXTRA_SPAWN (SPAWN_EXTRA_COST/RENT_PER_CYCLE) 

#define PAY 1000
#define BIG 1000000

typedef struct {
  MobTact tMob;
  Cash msgCash;
  Cash mobCash;
  CpuBid bid;
  jmp_buf jmpbuf;
} Core;

extern MobTact 
  tInvestor, tSales, 
  tMemCosts, tMemLost, 
  tCpuCosts, tCpuLost, tCpuFines, 
  tUndeliverables;

//typedef int RunResult;
//typedef struct { bool ran; RunResult res; } DeliverResult;
//typedef struct { Cash cost; RunResult res; } CostAndResult;

//typedef CostAndResult (*Runner)(Mob * pMob);
typedef void (*WithPayload)(MsgPayload * pPayload);

bool openTank();
void closeTank(FATE);
void showTank(void);
void showMobTact(MobTact t);

bool spawn(Core * pCore, Cash cash, WithMobBody stuffBody, MobTact * ptChild);
bool post (Core * pCore, Cash cash, CpuBid bid, MobTact rcvr, WithPayload stuff);
void choose(void);


Cash mobPayMob(MobTact tFrom, MobTact tTo, Cash amt);
Cash msgPayMob(MsgIx iMsg, MobTact tMob, Cash amt);
Cash msgPayMobAll(MsgIx iMsg, MobTact tMob);

typedef struct {double spawnThresh; double payMsg; double bid; } Avg;
void smple(MobBody* pB);
extern Avg avg;
