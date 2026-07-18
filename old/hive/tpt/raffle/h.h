#pragma once

#include "XX_pile/1.h"
#include "XX_hotel/h.h"
#include "YY"
#include "ZZ_pile/1.h"

typedef void (*WithXXTicket)(XXTicket * pTicket);
typedef bool (*XXBIP)(XXIx i, XXTicket * pTicket);

typedef bool   (*XXRaffleOpen)   (void);
typedef void   (*XXRafflePlay)   (Cash cash, Weight w, WithXXTicket stuff);
//typedef void   (*XXRaffleEnrich) (XXIx iWho, Cash amt);
typedef Cash   (*XXRaffleRob)    (XX * pXX);
//typedef Cash   (*XXRaffleRobUpTo) (XXIx i, Cash limit);
//typedef Cash   (*XXRaffleCancel) (XXIx i);
typedef bool   (*XXRaffleEmpty)  (void);
typedef bool   (*XXRaffleDraw)   (void);
typedef void   (*XXRaffleClose)  (FATE fate);
typedef void   (*XXRaffleShow)   (void);
typedef Ix     (*XXRaffleCount)  (void);
typedef bool   (*XXRaffleCheck)  (void);
typedef void   (*XXRaffleKill)   (void);
typedef void   (*XXRaffleQuit)   (void);

typedef struct {
  XXRaffleOpen open;
  XXRafflePlay play;
//  XXRaffleEnrich enrich;
  XXRaffleRob rob;
  //XXRaffleRobUpTo robUpTo;
//  XXRaffleCancel cancel;
  XXRaffleEmpty empty;
  XXRaffleDraw draw;
  XXRaffleClose close;
  XXRaffleShow show;
  XXRaffleCount count;
  XXRaffleCheck check;
  XXRaffleKill kill;
  XXRaffleQuit quit;
} XXRaffle;

extern void onXXRaffleDispatch(XXIx i, XX * pMsg, VV claim, VV unlock, VV rob);
                                  
extern XXRaffle raffleOfXXs;



