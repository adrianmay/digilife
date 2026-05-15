// #pragma once

//#include "XX_pile/1.h"
//#include "XX_hotel/.h"
//#include "XX_pile/2.h"

#include "XX_pile/1.h"

extern char XXGOD[];
typedef bool        (*XXHotelOpen)(void);
typedef XXIx        (*XXHotelAlloc)(Cash cash, XX ** pp, bool * pRecycled);
typedef XX *        (*XXHotelGet)(XXIx i); 
typedef void        (*XXHotelTransfer)(Cash amt, XXIx iFrom, XXIx iTo);
typedef void        (*XXHotelReview)(XXIx i);
typedef Cash        (*XXHotelRobber)(XXIx i);
typedef void        (*XXHotelKiller)(void);
typedef Ix          (*XXHotelCount)(void); 
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

