#include "XX_pile/1.h"
#include "YY"

typedef bool        (*XXRaffleOpen)   (Cash cash, XXIx * pI);
typedef void        (*XXRaffleClose)  (FATE fate);
typedef XXIx (*XXRaffleEnter)  (Cash cash, XXIx iDonor, Weight w, XXTicket * pTicket);
typedef Cash        (*XXRaffleCancel) (XXIx i);
typedef bool        (*XXRaffleDraw)   (XXTicket * pTicket, Cash * pCash);
typedef void        (*XXRaffleShow)   (void);

typedef struct {
  XXRaffleOpen open;
  XXRaffleEnter enter;
  XXRaffleCancel cancel;
  XXRaffleDraw draw;
  XXRaffleClose close;
  XXRaffleShow show;
} XXRaffle;

extern XXRaffle raffleOfXXs;



