#ifndef MsgTicket_h
#define MsgTicket_h

#include "MobBulk_pile/1.h"
typedef struct {
  CpuBid cpubid; 
  MobBulkIndex iRcvr; Name nRcvr;
  MobBulkIndex iSndr; Name nSndr; 
  MobBulkIndex iCtnt; Name nCtnt; 
  uint16_t ctntStart; uint16_t ctntEnded;}
  MsgTicket;

#endif  

