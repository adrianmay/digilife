
typedef void (*XXHotelOpen)();
typedef XXBulkIndex (*XXHotelAlloc)(Cash cash, XXBulk ** ppBulk);
typedef void (*XXHotelReview)(XXBulkIndex i);
typedef void * (*XXHotelKiller)(void * p);
typedef void (*XXHotelClose)(FATE fate);

typedef struct {
  XXHotelOpen open;
  XXHotelAlloc alloc;
  XXHotelReview review;
  XXHotelKiller killer;
  XXHotelClose close;
} XXHotel;

extern XXHotel hotelOfXXs;

