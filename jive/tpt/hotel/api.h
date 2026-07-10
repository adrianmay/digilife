typedef struct {XXIx i; Nick n; } XXTact;
typedef void (*V_XXP)(XX *);

void   hotelOfXXs_show(void);
void   hotelOfXXs_showXX(XXTact, XX *);
bool   hotelOfXXs_open(void);
XXTact hotelOfXXs_admit(Cash cash, bool isGod, V_XXP stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
XX *   hotelOfXXs_grab(XXTact, Cash *);
XX *   hotelOfXXs_grabIx(XXIx, Cash *);
void   hotelOfXXs_drop(XXIx, Cash);
void   hotelOfXXs_raid(void);
Ix     hotelOfXXs_count(void);
void   hotelOfXXs_close(Fate);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
extern void onXXsExtinct(void); // Provide this.
extern void onXXHotelGoDie(XXIx, XX * pXX); // Provide this.
extern void onXXRentCollected(Cash cash); // Provide this.
extern void onXXRentDefaulted(Cash cash); // Provide this.

extern const size_t billableXXSize;
                                         


