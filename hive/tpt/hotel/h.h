// #pragma once

//#include "XXBulk_pile/1.h"
//#include "XX_hotel/Bulk.h"
//#include "XXBulk_pile/2.h"

#include "XXBulk_pile/1.h"

typedef bool        (*XXHotelOpen)();
typedef XXBulkIndex (*XXHotelAlloc)(Cash cash, XXBulk ** ppBulk, bool * pRecycled);
typedef XXBulk *    (*XXHotelGet)(XXBulkIndex i); 
typedef void        (*XXHotelReview)(XXBulkIndex i);
typedef Cash        (*XXHotelRobber)(XXBulkIndex i);
typedef void        (*XXHotelKiller)();
typedef Index       (*XXHotelCount)(); 
typedef void        (*XXHotelClose)(FATE fate);
typedef void        (*XXHotelShow)();

typedef struct {
  XXHotelOpen open;
  XXHotelAlloc alloc;
  XXHotelGet get;
  XXHotelReview review;
  XXHotelRobber rob;
  XXHotelKiller kill;
  XXHotelCount count;
  XXHotelClose close;
  XXHotelShow show;
} XXHotel;

extern void onXXsExtinct(); // Provide this.
extern XXHotel hotelOfXXs;

