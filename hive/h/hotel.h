#include "sleep.h"
#include "meap.h"
#include "globals.h"
#include "assert.h"

extern Tocks    wrapSubTocksU(Tocks a, Tocks b);
extern TockDiff wrapSubTocksS(Tocks a, Tocks b);
extern Tocks    wrapAddTocks(Tocks a, Tocks b);
void updateTocks();
typedef Tocks (*KILLER)(Score thresh);
bool rentCollector(KILLER killer);

#define MAKEHOTEL1(TYP) \
  MAKEPILE1(TYP) \
  MAKEMEAP1(TYP##Meap) \
  typedef struct { Tocks tocks; TYP##Index who; } TYP##Meap; \
  typedef struct { Cash cash; Tocks lastPaidRent; TYP##MeapIndex meap; } TYP##Rent; \

// TYP must have Rent rent

// getScore just looks at tocks in the meap. This could have been in the animal but I think this
//   way suffers fewer cache misses. 
//     To keep the meap in 64 bits, I made tocks 32 bit. 
//     It can wrap, but we don't want extremely rich animals
//       who would die early because of tocks wrapping.
//     It's not tragic for things to be slightly late in dying.  
// onMove just tells the animal where its meap is.
//   then goes back to sleep until the next expected death. 
//     For that it needs to know the thread id of said thread. This could be global. 
//     Might want to suppress waking during the killing phase of housekeeping.
// onNew updates the current tock then sets up the animal's rent field and the meap's tocks and animal index.
//   The calling code will make the animal first with some money, then call meapInsert with the animal index
//     as the hint.
// Appeal is needed because of tock wrapping.

#define MAKEHOTEL2(TYP,LIM) \
  pthread_t TYP##HotelSleeperThread; \
  MAKEPILE2(TYP,LIM) \
  MAKEMEAP2(TYP##Meap,LIM) \
  TYP##Index reserve##TYP(Cash cash, TYP ** pB) { \
    updateTocks(); \
    TYP##Index iB = alloc##TYP(pB); \
    TYP##Rent * pRent = &((*pB)->rent); \
    pRent->cash = cash; \
    pRent->lastPaidRent = pg->lastKnownTock; \
    TYP##MeapIndex iM; \
    TYP##Meap * pM; \
    if (meapInsert##TYP##Meap(&iM, &pM, iB.i)) \
      wake(TYP##HotelSleeperThread); \
    pM->who = iB; \
    return iB; \
  } \
  Score getScore##TYP##Meap(TYP##Meap * pMeap) { return pMeap->tocks; } \
  bool meapAppeal##TYP##Meap(TYP##Meap * pMeap) { return false; } \
  extern void mourn##TYP##Meap(TYP##Meap *); \
  void onMove##TYP##Meap(TYP##Meap * pMeap, TYP##MeapIndex i) { get##TYP(pMeap->who)->rent.meap = i; } \
  void onNew##TYP##Meap(TYP##MeapIndex iMeap, uint32_t hint) { \
    TYP##Meap * pMeap = get##TYP##Meap(iMeap); \
    TYP##Index iTyp = (TYP##Index){hint}; \
    TYP * pTyp = get##TYP(iTyp); \
    TYP##Rent * pRent = &pTyp->rent; \
    pRent->meap = iMeap; \
    pMeap->who = iTyp; \
    pMeap->tocks = pRent->cash/pg->groatsPerTock + pg->lastKnownTock + 1; \
  } \
  bool addCash##TYP(TYP##Index i, Cash c) { \
    return true; \
  } \
  bool charge##TYP##Rent(TYP##Index i) { \
    TYP * pB = get##TYP(i); \
    TYP##Rent * pRent = &pB->rent; \
    TockDiff tcks = wrapSubTocksS(pg->lastKnownTock, pRent->lastPaidRent); \
    assertCond(tcks, >0); \
    Cash bill = pg->groatsPerTock*tcks; \
    if (bill >= pRent->cash) return false; \
    pRent->cash -= bill; \
    pRent->lastPaidRent = pg->lastKnownTock; \
    return true; \
  } \
  bool open##TYP##Hotel() { open##TYP##Pile(); return open##TYP##MeapPile(); } \
  void close##TYP##Hotel(int rm) { close##TYP##Pile(rm); close##TYP##MeapPile(rm); } \
  Tocks kill##TYP##s(Score thresh) { \
    int c; \
    TYP##Meap meap; \
    while ( (c=chomp##TYP##Meap(thresh, &meap))==1 ) { \
      mourn##TYP##Meap(&meap); \
    } \
    if (c==-1) return 0; \
    return meap.tocks; \
  } 
  

