#pragma once

#include "XX_pile/1.h"
#include "YY"
#include "ZZ_pile/1.h"

typedef void (*WithXXTicket)(XXTicket * pTicket);
typedef bool (*OnChosen)(XXIx i, XXTicket * pTicket);

typedef bool   (*XXRaffleOpen)   (void);
typedef XXIx   (*XXRafflePlay)   (Cash cash, Weight w, WithXXTicket stuff);
typedef void   (*XXRaffleEnrich) (XXIx iWho, Cash amt);
typedef Cash   (*XXRaffleRob)    (XXIx i);
typedef Cash   (*XXRaffleCancel) (XXIx i);
typedef bool   (*XXRaffleEmpty)  (void);
typedef bool   (*XXRaffleDraw)   (OnChosen);
typedef void   (*XXRaffleClose)  (FATE fate);
typedef void   (*XXRaffleShow)   (void);
typedef Ix     (*XXRaffleCount)  (void);
typedef bool   (*XXRaffleCheck)  (void);
typedef void   (*XXRaffleKill)   (void);
typedef void   (*XXRaffleQuit)   (void);

typedef struct {
  XXRaffleOpen open;
  XXRafflePlay play;
  XXRaffleEnrich enrich;
  XXRaffleRob rob;
  XXRaffleCancel cancel;
  XXRaffleEmpty empty;
  XXRaffleDraw draw;
  XXRaffleClose close;
  XXRaffleShow show;
  XXRaffleCount count;
  XXRaffleCheck check;
  XXRaffleKill kill;
  XXRaffleQuit quit;
} XXRaffle;

extern void onXXRaffleGoDie(XXIx); // Provide this.
                                  
extern XXRaffle raffleOfXXs;



