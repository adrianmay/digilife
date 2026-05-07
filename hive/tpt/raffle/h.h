#include "XXBulk_pile/1.h"
#include "XX_raffle/structs.h"

typedef void        (*XXRaffleOpen)   ();
typedef void        (*XXRaffleClose)  (FATE fate);
typedef XXBulkIndex (*XXRaffleEnter)  (Cash cash, Weight w, XXTicket * pTicket);
typedef Cash        (*XXRaffleCancel) (XXBulkIndex i);
typedef Cash        (*XXRaffleDraw)   (XXTicket * pTicket);

typedef struct {
  XXRaffleOpen open;
  XXRaffleEnter enter;
  XXRaffleCancel cancel;
  XXRaffleDraw draw;
  XXRaffleClose close;
} XXRaffle;

extern XXRaffle raffleOfXXs;

//XXBulkIndex enter(Cash cash, Weight w, XXTicket * pTicket);
//Cash cancel(XXBulkIndex i);
//Cash draw(XXTicket * pTicket);


