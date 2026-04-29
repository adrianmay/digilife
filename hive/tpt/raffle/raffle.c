#include "Msg_raffle/h.h"
#include "XXBomb_pile/1.h"
#include "XX_hotel/Bulk.h"
#include "XXBulk_pile/2.h"
#include "XX_hotel/h.h"
static XXBulkIndex parent(XXBulkIndex i) {return ( XXBulkIndex ){ (i.i-1)/2 };}
static XXBulkIndex left  (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 1 };}
static XXBulkIndex right (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 2 };}

XXBulkIndex enter(Cash cash, Weight w, XXTicket * pTicket) {
  XXBulk * pBulk;
  hotelOfXXs.alloc(cash, &pBulk);
  return badXXBulkIndex;
}

Cash cancel(XXBulkIndex i) {
  return 0;
}

Cash draw(XXTicket * pTicket) {
  return 0;
}


