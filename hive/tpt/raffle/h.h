#include "XXBulk_pile/1.h"
#include "YY"

typedef bool        (*XXRaffleOpen)   ();
typedef void        (*XXRaffleClose)  (FATE fate);
typedef XXBulkIndex (*XXRaffleEnter)  (Cash cash, Weight w, XXTicket * pTicket);
typedef Cash        (*XXRaffleCancel) (XXBulkIndex i);
typedef bool        (*XXRaffleDraw)   (XXTicket * pTicket, Cash * pCash);
typedef void        (*XXRaffleShow)   ();

typedef struct {
  XXRaffleOpen open;
  XXRaffleEnter enter;
  XXRaffleCancel cancel;
  XXRaffleDraw draw;
  XXRaffleClose close;
  XXRaffleShow show;
} XXRaffle;

extern XXRaffle raffleOfXXs;



