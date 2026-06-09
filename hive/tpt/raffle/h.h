#pragma once

#include "XX_pile/1.h"
#include "YY"
#include "ZZ_pile/1.h"

typedef void (*WithXXTicket)(XXTicket * pTicket);
typedef void (*WithXXTicketAndCash)(XXTicket * pTicket, Cash cash);

typedef bool   (*XXRaffleOpen)   (void);
typedef void   (*XXRaffleClose)  (FATE fate);
typedef XXIx   (*XXRaffleEnter)  (Cash cash, Weight w, WithXXTicket stuff);
typedef Cash   (*XXRaffleCancel) (XXIx i);
typedef bool   (*XXRaffleEmpty)  (void);
typedef bool   (*XXRaffleDraw)   (WithXXTicketAndCash enjoyTicket);
typedef void   (*XXRaffleShow)   (void);
typedef Ix     (*XXRaffleCount)  (void);
typedef bool   (*XXRaffleCheck)  (void);
typedef void   (*XXRaffleKiller) (void);
typedef void   (*XXRaffleQuit)   (void);

typedef struct {
  XXRaffleOpen open;
  XXRaffleEnter enter;
  XXRaffleCancel cancel;
  XXRaffleEmpty empty;
  XXRaffleDraw draw;
  XXRaffleClose close;
  XXRaffleShow show;
  XXRaffleCount count;
  XXRaffleCheck check;
  XXRaffleKiller kill;
  XXRaffleQuit quit;
} XXRaffle;

extern XXRaffle raffleOfXXs;



