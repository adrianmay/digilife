#include "XX_pile/XX.h"
#include "XX_pile/2.h"

// The X param is the type of the MEAP element, cos we aren't yet suggesting any relationship to some animal pile.
// The underlying pile where this lives should be instantiated already.

typedef enum {Extinct, Idle, Killed} Chomped;

typedef bool    (*XXMeapInsert)   (XXIndex *pI, XX ** pNew, Index hint); 
typedef bool    (*XXMeapEditWhen) (XXIndex i, Score when);
typedef bool    (*XXMeapErase)    (XXIndex i);
typedef Chomped (*XXMeapChomp)    (Score thresh, XX * p);
typedef bool    (*XXCheckOrdered) ();
typedef Index   (*XXMeapSize)     ();
//
// typedef enum {EXTINCT=-1, IDLE, } ChompResult;
// typedef ChompResult (*XXMeapChomp)(Score score, XXIndex * i);

typedef struct {
  XXMeapInsert insert; // New member.
  XXMeapEditWhen editWhen; // Member's score might have changed.
  XXMeapErase  erase; // Delete it.
  XXMeapChomp chomp; //Check and eat 0-1 things
  XXCheckOrdered checkOrdered; //Just for testing.
  XXMeapSize size; // How many members.
} XXMeap;


extern XXMeap meapOfXXs;

// The caller should define these:

extern void  onNewXX(XXIndex i, Index hint); // Hint is probably the index of a corresponding other thing.
extern void  onMoveXX(XX * p, XXIndex to); // Update records of where p's meap is.

