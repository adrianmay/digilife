
typedef struct {XXIx i; Nick n; } XXTact;

typedef void (*V_XXP)(XX *);
typedef void (*V_XXBodyP)(XXBody *);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
                                     
typedef bool        (*XXHotelOpen)(void);
typedef XXTact      (*XXHotelAdmit)(Cash cash, bool isGod, V_XXBodyP stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
typedef XX *        (*XXHotelGet)(XXIx i);
typedef XX *        (*XXHotelGrab)(XXTact);
typedef XX *        (*XXHotelGrabIx)(XXIx);
typedef void        (*XXHotelDrop)(XXIx);
typedef void        (*XXHotelRicher)(XX*, Cash);
typedef Cash        (*XXHotelPoorer)(XX*, Cash, Terms);
typedef Cash        (*XXHotelRob)(XX * pXX);
typedef void        (*XXHotelCollectRent)(XX * pXX, bool updateBomb);
typedef void        (*XXHotelVV)(void);
typedef void        (*XXHotelVI)(int expectExcess);
typedef Ix          (*XXHotelCount)(void);
typedef void        (*XXHotelClose)(Fate fate);
typedef void        (*XXHotelForAll)(bool u, V_XXI_XXP);
typedef void        (*XXHotelShow)(void);
typedef void        (*XXHotelShowXX)(XXTact, XX * p);

typedef struct {
  XXHotelOpen open;
  XXHotelAdmit admit;
  XXHotelGet get;
  XXHotelGrab grab;
  XXHotelGrabIx grabIx;
  XXHotelDrop drop;
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

