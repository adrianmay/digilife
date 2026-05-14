// #pragma once

//#include "XXBulk_pile/1.h"
//#include "XX_hotel/Bulk.h"
//#include "XXBulk_pile/2.h"

#include "XXBulk_pile/1.h"

extern char XXGOD[];
typedef bool        (*XXHotelOpen)(Cash cash); // Creates animal with this much cash when virgin pile
typedef XXBulkIx (*XXHotelAlloc)(Cash cash, XXBulkIx iDonor, XXBulk ** ppBulk, bool * pRecycled);
typedef XXBulk *    (*XXHotelGet)(XXBulkIx i); 
typedef void        (*XXHotelTransfer)(Cash amt, XXBulkIx iFrom, XXBulkIx iTo);
typedef void        (*XXHotelReview)(XXBulkIx i);
typedef Cash        (*XXHotelRobber)(XXBulkIx i);
typedef void        (*XXHotelKiller)(void);
typedef Ix       (*XXHotelCount)(void); 
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

