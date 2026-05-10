#include "XXBulk_pile/1.h"
#include "YY"

typedef bool        (*XXRaffleOpen)   (Cash cash, XXBulkIndex * pI);
typedef void        (*XXRaffleClose)  (FATE fate);
typedef XXBulkIndex (*XXRaffleEnter)  (Cash cash, XXBulkIndex iDonor, Weight w, XXTicket * pTicket);
typedef Cash        (*XXRaffleCancel) (XXBulkIndex i);
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



