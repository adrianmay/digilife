
// The X param is the type of the MEAP element, cos we aren't yet suggesting any relationship to some animal pile.
// The underlying pile where this lives should be instantiated already.

typedef enum {Extinct, Idle, Killed} Chomped;

bool    meapOfXXs_open      (void);
void    meapOfXXs_close     (Fate fate);
bool    meapOfXXs_insert    (Tocks expiry, Ix hint, XXIx * pI);
XX *    meapOfXXs_get       (XXIx i);
bool    meapOfXXs_editTocks (XXIx i, Score when);
bool    meapOfXXs_erase     (XXIx i);
Chomped meapOfXXs_chomp     (Score thresh, XX * p, int pseudoAnimals);
void    meapOfXXs_forAll    (V_I_P);
Ix      meapOfXXs_count     (void);
Ix      meapOfXXs_rec       (void);
void    meapOfXXs_show      (void);
bool    meapOfXXs_check     (void);
void    showXX              (XXIx i, XX * pXX);

extern const XXIx badXXIx;

// The caller should define these:
extern void onXXMeap_move(XX * p, XXIx to); // Update records of where p's meap is.
extern void onXXMeap_new(XX * p, Ix hint); // Update records of where p's meap is.

