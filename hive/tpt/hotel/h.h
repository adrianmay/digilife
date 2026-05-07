#pragma once

#include "XX_hotel/Bulk.h"
#include "XXBulk_pile/2.h"

typedef void (*XXHotelOpen)();
typedef XXBulkIndex (*XXHotelAlloc)(Cash cash, XXBulk ** ppBulk);
typedef void (*XXHotelReview)(XXBulkIndex i);
typedef Cash (*XXHotelRobber)(XXBulkIndex i);
typedef void (*XXHotelKiller)();
typedef void (*XXHotelClose)(FATE fate);
typedef void (*XXHotelShow)();

typedef struct {
  XXHotelOpen open;
  XXHotelAlloc alloc;
  XXHotelReview review;
  XXHotelRobber rob;
  XXHotelKiller kill;
  XXHotelClose close;
  XXHotelShow show;
} XXHotel;

extern void onXXsExtinct(); // Provide this.
extern XXHotel hotelOfXXs;

