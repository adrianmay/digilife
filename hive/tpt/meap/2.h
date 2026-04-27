#include "XX_pile/XX.h"
#include "XX_pile/2.h"

// The X param is the type of the MEAP element, cos we aren't yet suggesting any relationship to some animal pile.
// The underlying pile where this lives should be instantiated already.

typedef bool (*XXMeapInsert)(XXIndex *pI, XX ** pNew, Index hint); 
typedef bool (*XXMeapReview)(XXIndex i);
typedef bool (*XXMeapErase)(XXIndex i);
typedef bool (*XXCheckOrdered)();
typedef Index (*XXMeapSize)();
//
// typedef enum {EXTINCT=-1, IDLE, } ChompResult;
// typedef ChompResult (*XXMeapChomp)(Score score, XXIndex * i);

typedef struct {
  XXMeapInsert insert; // New member.
  XXMeapReview review; // Member's score might have changed.
  XXMeapErase  erase; // Delete it.
  XXCheckOrdered checkOrdered; //Just for testing.
  XXMeapSize size; // How many members.
} XXMeap;


extern XXMeap meapOfXXs;

// The caller should define these:

extern Score getXXScore(XX *);
extern void  onNewXX(XXIndex i, Index hint); // Hint is probably the index of a corresponding other thing.
extern void  onMoveXX(XX * p, XXIndex to); // Update records of where p's meap is.

