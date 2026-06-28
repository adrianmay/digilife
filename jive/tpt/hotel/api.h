
typedef struct {XXIx i; Nick n; } XXTact;

#define NICK_FLAG_BUSY   0x80000000
#define NICK_FLAG_DOOMED 0x40000000
// This is part of the name:
#define NICK_NAME_GOD    0x20000000
// When you only want the multithreading flags:
#define NICK_FLAG_MASK (NICK_FLAG_BUSY | NICK_FLAG_DOOMED)
// When you want the whole existing name:
#define NICK_NAME_READ_MASK (~(NICK_FLAG_MASK))
// When you make up a random new name:    
#define NICK_NAME_RAND_MASK (NICK_NAME_READ_MASK & (!NICK_NAME_GOD) )

typedef void (*V_XXP)(XX *);
typedef void (*V_XXBodyP)(XXBody *);

extern void onXXRentCollected(Cash rent); // Provide this
extern void onXXRentDefaulted(Cash rent); // Provide this
                                     
typedef bool        (*XXHotelOpen)(void);
typedef XXTact      (*XXHotelAdmit)(Cash cash, bool isGod, V_XXBodyP stuff, XX ** pp, bool * pRecycled); //Cash = 0 -> God
typedef XX *        (*XXHotelGet)(XXIx i);
typedef Woth        (*XXHotelWith)(XXTact, V_XXP);
typedef Woth        (*XXHotelWithIx)(XXIx, V_XXP);
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

