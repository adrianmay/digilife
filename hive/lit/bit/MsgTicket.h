#pragma once

#include "Mob_pile/1.h"


typedef struct {MobIx i; Nick n; } MobTact;

typedef struct {
} MsgPayload;

typedef struct {
  CpuBid cpuBid;
  MobTact rcvr;
  MobTact sndr;
  MsgPayload payload;
//  MobIx iCtnt; // Name nCtnt;
//  uint16_t ctntStart; uint16_t ctntEnded;
} MsgTicket;

void showMsgTicket(MsgTicket * p);
void showMobTact(MobTact t);



