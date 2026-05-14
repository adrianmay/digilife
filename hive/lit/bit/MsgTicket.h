// #pragma once

#include "MobBulk_pile/1.h"
typedef struct {
  CpuBid cpubid; 
  MobBulkIx iRcvr; Name nRcvr;
  MobBulkIx iSndr; Name nSndr; 
  MobBulkIx iCtnt; Name nCtnt; 
  uint16_t ctntStart; uint16_t ctntEnded;
} MsgTicket;

void showMsgTicket(MsgTicket * p);


