
typedef void (*WithXX)(XX * pXX);
typedef bool (*XXBIP)(XXIx i, XX * pXX);

bool   raffleOfXXs_open   (void);
void   raffleOfXXs_close  (Fate fate);
void   raffleOfXXs_quit   (void);
void   raffleOfXXs_play   (Cash cash, Weight w, WithXX stuff);
bool   raffleOfXXs_draw   (void);
void   raffleOfXXs_show   (void);
Ix     raffleOfXXs_count  (void);
bool   raffleOfXXs_empty  (void);

extern void onXXRaffleDispatch(XX * pMsg, Cash c, V claim, V unlock);

//typedef bool   (*XXRaffleCheck)  (void);
//typedef Cash   (*XXRaffleRob)    (XX * pXX);
//typedef void   (*XXRaffleKill)   (void);
//typedef void   (*XXRaffleEnrich) (XXIx iWho, Cash amt);
//typedef Cash   (*XXRaffleRobUpTo) (XXIx i, Cash limit);
//typedef Cash   (*XXRaffleCancel) (XXIx i);

