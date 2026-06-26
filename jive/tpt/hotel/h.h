
typedef struct {XXIx i; Nick n; } XXTact;

#define NICK_BUSY 0x80000000
#define NICK_DOOMED 0x40000000
typedef void (*WithXX)(XX *);
typedef void (*WithXXBody)(XXBody *);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
                                     
typedef bool        (*XXHotelOpen)(void);
typedef XXTact      (*XXHotelAdmit)(Cash cash, WithXXBody stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
typedef XX *        (*XXHotelGet)(XXIx i);
typedef Woth        (*XXHotelWith)(XXTact, WithXX);
typedef Woth        (*XXHotelWithIx)(XXIx, WithXX);
typedef void        (*XXHotelRicher)(XX*, Cash);
typedef Cash        (*XXHotelPoorer)(XX*, Cash, Terms);
typedef Cash        (*XXHotelRob)(XX * pXX);
typedef void        (*XXHotelCollectRent)(XX * pXX, bool updateBomb);
typedef void        (*XXHotelVV)(void);
typedef void        (*XXHotelVI)(int expectExcess);
typedef Ix          (*XXHotelCount)(void);
typedef void        (*XXHotelClose)(Fate fate);
typedef void        (*XXHotelForAll)(bool u, XXVIP);
typedef void        (*XXHotelShow)(void);
typedef void        (*XXHotelShowXX)(XXTact, XX * p);

typedef struct {
  XXHotelOpen open;
  XXHotelAdmit admit;
  XXHotelGet get;
  XXHotelWith with;
  XXHotelWithIx withIx;
  XXHotelRicher richer;
  XXHotelPoorer poorer;
  XXHotelRob rob; 
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
extern void onXXHotelGoDie(XXIx, XX * pXX); // Provide this.
extern void onXXRentCollected(Cash cash); // Provide this.
extern void onXXRentDefaulted(Cash cash); // Provide this.
extern XXHotel hotelOfXXs;

extern const size_t billableXXSize;

