#include "time.h"
#include "global.h"
#include "meap.h"

extern Tocks    wrapSubtractTocks(Tocks a, Tocks b);
extern Tocks    wrapAddTocks(Tocks a, Tocks b);
void updateTocks();
typedef Tocks (*KILLER)();
void rentCollector(KILLER killer);

#define MAKERENT1(TYP) \
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

#define MAKERENT2(TYP,LIM) \
  MAKEPILE2(TYP,LIM) \
  MAKEMEAP2(TYP##Meap,LIM) \
  Score getScore##TYP##Meap(TYP##Meap * pMeap) { return pMeap->tocks; } \
  extern void mourn##TYP##Meap(TYP##Meap *); \
  void onMove##TYP##Meap(TYP##Meap * pMeap, TYP##MeapIndex i) { get##TYP(pMeap->who)->rent.meap = i; } \
  void onNew##TYP##Meap(TYP##MeapIndex iMeap, uint32_t hint) { \
    updateTocks(); \
    TYP##Meap * pMeap = get##TYP##Meap(iMeap); \
    TYP##Index iTyp = (TYP##Index){hint}; \
    TYP * pTyp = get##TYP(iTyp); \
    TYP##Rent * pRent = &pTyp->rent; \
    pRent->lastPaidRent = pg->lastKnownTock; \
    pRent->meap = iMeap; \
    pMeap->who = iTyp; \
    pMeap->tocks = pRent->cash/pg->groatsPerTock + pg->lastKnownTock + 1; \
  } \
  bool openRent##TYP##s() { open##TYP##Pile(); return open##TYP##MeapPile(); } \
  void closeRent##TYP##s(bool rm) { close##TYP##Pile(rm); close##TYP##MeapPile(rm); } \
  void hideRent##TYP##s() { hide##TYP##Pile(); hide##TYP##MeapPile(); } \
  Tocks kill##TYP##s(Score thresh) { \
    int c; \
    TYP##Meap meap; \
    while ( (c=chomp##TYP##Meap(thresh, &meap))==1 ) { \
      mourn##TYP##Meap(&meap); \
    } \
    if (c==-1) return 0; \
    return meap.tocks; \
  }
  

