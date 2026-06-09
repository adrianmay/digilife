#include "Msg_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "bit/MsgTicket.h"

extern MobTact 
  tInvestor, tSales, tCostsCpu, tCostsMem,
  tRentDefaults, tUndeliverables;

typedef int RunResult;
typedef struct { bool ran; RunResult res; } DeliverResult;
typedef struct { Cash cost; RunResult res; } CostAndResult;

typedef CostAndResult (*Runner)(Mob * pMob);
typedef void (*WithBody)(MobBody * pMobBody);
typedef void (*WithPayload)(MsgPayload * pPayload);

bool openTank();
void closeTank(FATE);
void showTank(void);
MobTact spawn(Cash cash, MobTact iParent, WithBody train);
MsgIx post(Cash cash, CpuBid bid, MobTact sndr, MobTact rcvr, WithPayload stuff);
DeliverResult deliver(Runner runner, Msg * pMsg);
