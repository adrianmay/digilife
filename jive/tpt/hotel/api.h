typedef struct {XXIx i; Nick n; } XXTact;
typedef void (*V_XXP)(XX *);

void   hotelOfXXs_show   (void);
void   hotelOfXXs_showXX (XXTact, XX *);
bool   hotelOfXXs_open   (void);
XXTact hotelOfXXs_admit  (Cash cash, bool isGod, V_XXP stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
XX *   hotelOfXXs_get    (XXIx); // Unsafe
XX *   hotelOfXXs_grab   (XXTact, Cash *);
XX *   hotelOfXXs_grabIx (XXIx, Cash *);
void   hotelOfXXs_drop   (XXIx, Cash);
void   hotelOfXXs_raid   (void);
Ix     hotelOfXXs_count  (void);
Ix     hotelOfXXs_top    (void);
Cash   hotelOfXXs_rent   (void);
void   hotelOfXXs_close  (Fate);

// Provide these:
extern void showXX                  (XXIx i, XX * p);
extern void onXXHotel_rentCollected (Cash rent);
extern void onXXHotel_rentDefaulted (Cash rent); 
extern void onXXHotel_extinct       (void); 
extern void onXXHotel_goDie         (XXIx, XX * pXX); // Provide this.

extern const size_t billableXXSize;

