#include "XX_pile/1.h"
#include "YY"
#include "ZZ_pile/1.h"

typedef bool   (*XXRaffleOpen)   (void);
typedef void   (*XXRaffleClose)  (FATE fate);
typedef XXIx   (*XXRaffleEnter)  (Cash cash, Weight w, XXTicket * pTicket);
typedef Cash   (*XXRaffleCancel) (XXIx i);
typedef bool   (*XXRaffleDraw)   (XXTicket * pTicket, Cash * pCash);
typedef void   (*XXRaffleShow)   (void);
typedef bool   (*XXRaffleCheck)  (void);

typedef struct {
  XXRaffleOpen open;
  XXRaffleEnter enter;
  XXRaffleCancel cancel;
  XXRaffleDraw draw;
  XXRaffleClose close;
  XXRaffleShow show;
  XXRaffleCheck check;
} XXRaffle;

extern XXRaffle raffleOfXXs;



