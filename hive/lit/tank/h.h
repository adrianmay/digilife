#pragma once

#include "Msg_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "bit/MsgTicket.h"

//typedef WithXXBody WithXXBody;

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

//MobTact spawn(Core * pCore, Cash cash, WithMobBody train);
//bool post(Cash cash, CpuBid bid, Mob * pSender, MobTact sndr, MobTact rcvr, WithPayload stuff);
void choose(void);


Cash mobPayMob(MobTact tFrom, MobTact tTo, Cash amt);
Cash msgPayMob(MsgIx iMsg, MobTact tMob, Cash amt);
Cash msgPayMobAll(MsgIx iMsg, MobTact tMob);

typedef struct {double spawnThresh; double payMsg; double bid; } Avg;
void smple(MobBody* pB);
extern Avg avg;
