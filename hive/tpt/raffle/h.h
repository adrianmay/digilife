#include "XXBulk_pile/1.h"
#include "YY"

typedef bool        (*XXRaffleOpen)   (Cash cash, XXBulkIx * pI);
typedef void        (*XXRaffleClose)  (FATE fate);
typedef XXBulkIx (*XXRaffleEnter)  (Cash cash, XXBulkIx iDonor, Weight w, XXTicket * pTicket);
typedef Cash        (*XXRaffleCancel) (XXBulkIx i);
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



