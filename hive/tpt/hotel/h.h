#pragma once

#include "XX_pile/1.h"
//#include "XX_hotel/h.h"
#include "XX_pile/2.h"

typedef enum {Exact, Ono, Rob} Terms;

typedef void (*WithXXBody)(XXBody *);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
                                     
typedef bool        (*XXHotelOpen)(void);
typedef XXIx        (*XXHotelAdmit)(Cash cash, WithXXBody stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
typedef XX *        (*XXHotelGet)(XXIx i);
typedef void        (*XXHotelRicher)(XXIx iWho, Cash amt);
typedef Cash        (*XXHotelPoorer)(XXIx iWho, Cash amt, Terms t);
typedef void        (*XXHotelCollectRent)(XXIx iWho);
typedef void        (*XXHotelVV)(void);
typedef void        (*XXHotelVI)(int expectExcess);
typedef Ix          (*XXHotelCount)(void);
typedef void        (*XXHotelClose)(FATE fate);
typedef void        (*XXHotelForAll)(bool u, XXVIP);
typedef void        (*XXHotelShow)(void);
typedef void        (*XXHotelShowXX)(XXIx i, XX * p);

typedef struct {
  XXHotelOpen open;
  XXHotelAdmit admit;
  XXHotelGet get;
  XXHotelRicher richer;
  XXHotelPoorer poorer;
  XXHotelCollectRent collectRent;
  XXHotelForAll forAll;
  XXHotelVV raid;
  XXHotelCount count;
  XXHotelVI checkHotel;
  XXHotelClose close;
  XXHotelShow show;
  XXHotelShowXX showXX;
} XXHotel;

extern void onXXsExtinct(void); // Provide this.
extern bool onXXHotelGoDie(XXIx, XX * pXX); // Provide this.
extern void onXXRentCollected(Cash cash); // Provide this.
extern void onXXRentDefaulted(Cash cash); // Provide this.
extern XXHotel hotelOfXXs;

extern const size_t billableXXSize;

