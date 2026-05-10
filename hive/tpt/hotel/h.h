// #pragma once

//#include "XXBulk_pile/1.h"
//#include "XX_hotel/Bulk.h"
//#include "XXBulk_pile/2.h"

#include "XXBulk_pile/1.h"

typedef bool        (*XXHotelOpen)(Cash cash, XXBulkIndex * pI); // Creates animal with this much cash when virgin pile
typedef XXBulkIndex (*XXHotelAlloc)(Cash cash, XXBulkIndex iDonor, XXBulk ** ppBulk, bool * pRecycled);
typedef XXBulk *    (*XXHotelGet)(XXBulkIndex i); 
typedef void        (*XXHotelTransfer)(Cash amt, XXBulkIndex iFrom, XXBulkIndex iTo);
typedef void        (*XXHotelReview)(XXBulkIndex i);
typedef Cash        (*XXHotelRobber)(XXBulkIndex i);
typedef void        (*XXHotelKiller)(void);
typedef Index       (*XXHotelCount)(void); 
typedef void        (*XXHotelClose)(FATE fate);
typedef void        (*XXHotelShow)(void);

typedef struct {
  XXHotelOpen open;
  XXHotelAlloc alloc;
  XXHotelGet get;
  XXHotelTransfer transfer;
  XXHotelReview review;
  XXHotelRobber rob;
  XXHotelKiller kill;
  XXHotelCount count;
  XXHotelClose close;
  XXHotelShow show;
} XXHotel;

extern void onXXsExtinct(void); // Provide this.
extern XXHotel hotelOfXXs;

