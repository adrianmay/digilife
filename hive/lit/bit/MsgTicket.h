#pragma once

#include "Mob_pile/1.h"

typedef struct {MobIx i; Ix n; } MobTact;

typedef struct {
  CpuBid cpuBid;
  MobTact rcvr;
  MobTact sndr;
//  MobIx iCtnt; // Name nCtnt;
//  uint16_t ctntStart; uint16_t ctntEnded;
} MsgTicket;

void showMsgTicket(MsgTicket * p);


