
typedef void (*WithXX)(XX * pXX);
typedef bool (*XXBIP)(XXIx i, XX * pXX);

bool      raffleOfXXs_open         (void);
void      raffleOfXXs_close        (Fate fate);
void      raffleOfXXs_quit         (void);
void      raffleOfXXs_play         (Cash cash, Weight w, WithXX stuff);
bool      raffleOfXXs_draw         (void);
void      raffleOfXXs_show         (void);
Ix        raffleOfXXs_count        (void);
double    raffleOfXXs_rec          (void);
TockPrice raffleOfXXs_rent         (void);
bool      raffleOfXXs_empty        (void);
//bool      raffleOfXXs_check        (void);
void      showXXTicket             (XXTicketIx i, XXTicket * pT);

// Provide this:
void showXX(XXIx i, XX * pXX);
// Return the correct amount of cash assuming rent paid until now...
extern Cash onXXRaffle_dispatch(XXIx i, XX * pMsg, Cash c, V claim, V unlock);
extern void onXXRaffle_extinct();

//typedef bool   (*XXRaffleCheck)  (void);
//typedef Cash   (*XXRaffleRob)    (XX * pXX);
//typedef void   (*XXRaffleKill)   (void);
//typedef void   (*XXRaffleEnrich) (XXIx iWho, Cash amt);
//typedef Cash   (*XXRaffleRobUpTo) (XXIx i, Cash limit);
//typedef Cash   (*XXRaffleCancel) (XXIx i);

