#include "XX_pile/2.h" //TODO: Do we need this?

// The X param is the type of the MEAP element, cos we aren't yet suggesting any relationship to some animal pile.
// The underlying pile where this lives should be instantiated already.

typedef enum {Extinct, Idle, Killed} Chomped;

typedef bool    (*XXMeapOpen)      (void);
typedef void    (*XXMeapClose)     (FATE fate);
typedef bool    (*XXMeapInsert)    (Tocks expiry, Ix hint, XXIx * pI);
typedef bool    (*XXMeapEditTocks) (XXIx i, Score when);
typedef bool    (*XXMeapErase)     (XXIx i);
typedef Chomped (*XXMeapChomp)     (Score thresh, XX * p, int pseudoAnimals);
typedef bool    (*XXCheckOrdered)  (void);
//typedef void    (*XXForAll)        (void (*)(Ix, void *));
typedef Ix      (*XXMeapSize)      (void);
typedef void    (*XXMeapShow)      (void);
//
// typedef enum {EXTINCT=-1, IDLE, } ChompResult;
// typedef ChompResult (*XXMeapChomp)(Score score, XXIx * i);

typedef struct {
  XXMeapOpen open;
  XXMeapClose close;
  XXMeapInsert insert; // New member.
  XXMeapEditTocks editTocks; // Member's score might have changed.
  XXMeapErase  erase; // Delete it.
  XXMeapChomp chomp; //Check and eat 0-1 things
  XXCheckOrdered check; //Just for testing.
//  XXForAll forAll; //Just for testing.
  XXMeapSize size; // How many members.
  XXMeapShow show;
} XXMeap;


extern XXMeap meapOfXXs;

// The caller should define this:
extern void  onMoveXX(XX * p, XXIx to); // Update records of where p's meap is.
extern void  onNewXX(XXIx i, Ix hint); // Update records of where p's meap is.

