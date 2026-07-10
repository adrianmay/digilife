typedef struct {XXIx i; Nick n; } XXTact;

typedef void (*V_XXP)(XX *);
typedef void (*V_XXBodyP)(XXBody *);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
                                     
bool   hotelOfXXs_open(void);
XXTact hotelOfXXs_admit(Cash cash, bool isGod, V_XXBodyP stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
void   hotelOfXXs_raid(void);
XX *   hotelOfXXs_get(XXIx i);
XX *   hotelOfXXs_grab(XXTact);
XX *   hotelOfXXs_grabIx(XXIx);
void   hotelOfXXs_drop(XXIx);
void   hotelOfXXs_richer(XX*, Cash);
Cash   hotelOfXXs_poorer(XX*, Cash, Terms);
Cash   hotelOfXXs_rob(XX * pXX);
void   hotelOfXXs_collectRent(XX * pXX);
Ix     hotelOfXXs_count(void);
void   hotelOfXXs_close(Fate fate);
void   hotelOfXXs_forAll(bool u, V_XXI_XXP);
void   hotelOfXXs_checkHotel(int expectExcess);
void   hotelOfXXs_show(void);
void   hotelOfXXs_showXX(XXTact, XX * p);

extern void onXXsExtinct(void); // Provide this.
extern void onXXHotelGoDie(XXIx, XX * pXX); // Provide this.
extern void onXXRentCollected(Cash cash); // Provide this.
extern void onXXRentDefaulted(Cash cash); // Provide this.

extern const size_t billableXXSize;

