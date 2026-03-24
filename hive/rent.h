#include "meap.h"

Tocks    wrapSubtractTocks(Tocks a, Tocks b);
Tocks    wrapAddTocks(Tocks a, Tocks b);

void updateTocks();

#define MAKERENT1(TYP) \
  MAKEPILE1(TYP) \
  MAKEMEAP1(TYP##Meap) \
  typedef struct { Tocks tocks; TYP##Index who; } TYP##Meap; \
  typedef struct { Cash cash; Tocks lastPaidRent; TYP##MeapIndex meap; } TYP##Rent; \

// TYP must have Rent rent

#define MAKERENT2(TYP,LIM) \
  MAKEPILE2(TYP,LIM) \
  MAKEMEAP2(TYP##Meap,LIM) \
  Score getScore##TYP##Meap(TYP##Meap * pMeap) { return pMeap->tocks; } \
  void onMove##TYP##Meap(TYP##Meap * pMeap, TYP##MeapIndex i) { get##TYP(pMeap->who)->rent.meap = i; } \
  void onNewLow##TYP##Meap(Score s) { wake(); } \
  void onNew##TYP##Meap(TYP##MeapIndex iMeap, uint32_t hint) { \
    updateTocks(); \
    TYP##Meap * pMeap = get##TYP##Meap(iMeap); \
    TYP##Index iTyp = (TYP##Index){hint}; \
    TYP * pTyp = get##TYP(iTyp); \
    TYP##Rent * pRent = &pTyp->rent; \
    pRent->lastPaidRent = g->lastKnownTock; \
    pRent->meap = iMeap; \
    pMeap->who = iTyp; \
    pMeap->tocks = pRent->cash/g->groatsPerTock + g->lastKnownTock + 1; \
  } \
  bool openRent##TYP##s() { open##TYP##Pile(); return open##TYP##MeapPile(); } \
  void closeRent##TYP##s(bool rm) { close##TYP##Pile(rm); close##TYP##MeapPile(rm); } \
  void hideRent##TYP##s() { hide##TYP##Pile(); hide##TYP##MeapPile(); } \
  

