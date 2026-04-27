

typedef void (*XXHotelOpen)();
typedef XXBulkIndex (*XXHotelAlloc)(Cash cash, XXBulk ** ppBulk);
typedef void (*XXHotelReview)(XXBulkIndex i);
typedef void (*XXHotelKiller)();
typedef void (*XXHotelClose)(FATE fate);

typedef struct {
  XXHotelOpen open;
  XXHotelAlloc alloc;
  XXHotelReview review;
  XXHotelKiller kill;
  XXHotelClose close;
} XXHotel;

extern XXHotel hotelOfXXs;

