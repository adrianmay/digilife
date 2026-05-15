#pragma once

#include "Mob_pile/1.h"
typedef struct {
  CpuBid cpubid; 
  MobIx iRcvr; Name nRcvr;
  MobIx iSndr; Name nSndr; 
  MobIx iCtnt; Name nCtnt; 
  uint16_t ctntStart; uint16_t ctntEnded;
} MsgTicket;

void showMsgTicket(MsgTicket * p);


