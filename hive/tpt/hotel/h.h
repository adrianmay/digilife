// #pragma once

//#include "XX_pile/1.h"
//#include "XX_hotel/.h"
//#include "XX_pile/2.h"

#include "XX_pile/1.h"

extern char XXGOD[];
typedef bool        (*XXHotelOpen)(void);
typedef XXIx        (*XXHotelAlloc)(Cash cash, XX ** pp, bool * pRecycled);
typedef XX *        (*XXHotelGet)(XXIx i);
typedef void        (*XXHotelEnrich)(XXIx iWho, Cash amt);
typedef bool        (*XXHotelChargeIfCan)(XXIx iWho, Cash amt);
typedef void        (*XXHotelReview)(XXIx i);
typedef Cash        (*XXHotelRobber)(XXIx i);
typedef void        (*XXHotelKiller)(void);
typedef Ix          (*XXHotelCount)(void);
typedef void        (*XXHotelClose)(FATE fate);
typedef void        (*XXHotelShow)(void);
typedef void        (*XXHotelShowXX)(XXIx i, XX * p);

typedef struct {
  XXHotelOpen open;
  XXHotelAlloc alloc;
  XXHotelGet get;
  XXHotelEnrich enrich;
  XXHotelChargeIfCan chargeIfCan;
  XXHotelReview review;
  XXHotelRobber rob;
  XXHotelKiller kill;
  XXHotelCount count;
  XXHotelClose close;
  XXHotelShow show;
  XXHotelShowXX showXX;
} XXHotel;

extern void onXXsExtinct(void); // Provide this.
extern void onXXKilled(XXIx); // Provide this.
extern XXHotel hotelOfXXs;

