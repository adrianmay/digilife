#pragma once

#include "XX_pile/1.h"
//#include "XX_hotel/h.h"
#include "XX_pile/2.h"

typedef void (*WithXXBody)(XXBody *);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
                                     
typedef bool        (*XXHotelOpen)(void);
typedef XXIx        (*XXHotelAdmit)(Cash cash, WithXXBody stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
typedef XX *        (*XXHotelGet)(XXIx i);
typedef void        (*XXHotelEnrich)(XXIx iWho, Cash amt);
typedef bool        (*XXHotelChargeIfCan)(XXIx iWho, Cash amt);
typedef void        (*XXHotelCollectRent)(XXIx iWho);
typedef Cash        (*XXHotelRob)(XXIx i);
typedef Cash        (*XXHotelRobUpTo)(XXIx i, Cash limit);
typedef void        (*XXHotelRaid)(void);
typedef Ix          (*XXHotelCount)(void);
typedef void        (*XXHotelClose)(FATE fate);
typedef void        (*XXHotelForAll)(bool u, XXVIP);
typedef void        (*XXHotelShow)(void);
typedef void        (*XXHotelShowXX)(XXIx i, XX * p);

typedef struct {
  XXHotelOpen open;
  XXHotelAdmit admit;
  XXHotelGet get;
  XXHotelEnrich enrich;
  XXHotelChargeIfCan chargeIfCan;
  XXHotelCollectRent collectRent;
  XXHotelForAll forAll;
  XXHotelRob rob;
  XXHotelRobUpTo robUpTo;
  XXHotelRaid raid;
  XXHotelCount count;
  XXHotelClose close;
  XXHotelShow show;
  XXHotelShowXX showXX;
} XXHotel;

extern void onXXsExtinct(void); // Provide this.
extern void onXXHotelGoDie(XXIx); // Provide this.
extern void onXXRentCollected(Cash cash); // Provide this.
extern void onXXRentDefaulted(Cash cash); // Provide this.
extern XXHotel hotelOfXXs;

extern const size_t billableXXSize;

