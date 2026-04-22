#include "XX_pile/XX.h"
#include "XX_pile/2.h"

// The X param is the type of the MEAP element, cos we aren't yet suggesting any relationship to some animal pile.
// The underlying pile where this lives should be instantiated already.

typedef bool (*XXMeapInsert)(XXIndex *pI, XX ** pNew, Index hint); 
typedef bool (*XXMeapReview)(XXIndex i);
typedef bool (*XXMeapRemove)(XXIndex i);
typedef bool (*XXCheckOrdered)();
//
// typedef enum {EXTINCT=-1, IDLE, } ChompResult;
// typedef ChompResult (*XXMeapChomp)(Score score, XXIndex * i);

typedef struct {
  XXMeapInsert insert;
  XXMeapReview review;
  XXMeapRemove remove;
  XXCheckOrdered checkOrdered;
} XXMeap;


extern XXMeap meapOfXXs;

// The caller should define these:

extern Score getXXScore(XX *);
extern void  onNewXX(XXIndex i, Index hint);
extern void  onMoveXX(XX * p, XXIndex to);

