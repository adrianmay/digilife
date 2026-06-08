#include "Msg_pile/2.h"
#include "Mob_hotel/h.h"
#include "Msg_pile/2.h"
#include "Msg_raffle/h.h"
#include "bit/MsgTicket.h"
//#define PAYLOAD_LEN 
//typedef struct { char p[PAYLOAD_LEN]; } Payload
typedef int RunResult;
typedef struct { bool ran; RunResult res; } DeliverResult;
typedef struct { Cash cost; RunResult res; } CostAndResult;

typedef CostAndResult (*Runner)(Mob * pMob);
typedef void (*WithBody)(MobBody * pMobBody);

extern MobTact tGod;
bool openTank();
void closeTank(FATE);
void showTank(void);
MobTact spawn(Cash cash, MobTact iParent, WithBody train);
MsgIx post(Cash cash, CpuBid bid, MobTact sndr, MobTact rcvr);
DeliverResult deliver(Runner runner, Msg * pMsg);
