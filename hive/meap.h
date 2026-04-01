#include "pile.h"

typedef void (*OnMove) (void *, Index);
typedef Score (*OnScore) (void *);
typedef void (*OnNewLow) (Score low);
typedef void (*OnNew) (Index iMeap, uint32_t hint);
// These callbacks customise the behaviour of a meap:
typedef struct { void * tmp; OnScore getScore; OnNew onNew; OnMove onMove; OnNewLow onNewLow; } MeapCallbacks;

void meapInsert(Pilehead * ph, MeapCallbacks * mc, void ** pNew, uint32_t hint);
void meapRemove(Pilehead * ph, MeapCallbacks * mc, Index iCur);
void meapReview(Pilehead * ph, MeapCallbacks * mc, Index iCur);

#define MAKEMEAP1(TYP) \
  MAKEPILE1(TYP) \

// You provide the stuff marked extern...
// Score is the thing that min heap minimises
// onMove might need to update something, in our case, the animal must know the meap index
// onNew sets up the fields of the meap (and animal) after we have the meap slot 
//   and before we fix the ordering of the meap.
// onNewLow is triggered when some change of cash or removal of dead stuff results in a new earliest expected death.
//   We will interrupt a sleep and re-start it with the new earliest.
// Otherwise it's mostly casted versions of the stuff in meap.c
// The tmp is used in swap in meap.c.

#define MAKEMEAP2(TYP, LIM) \
  MAKEPILE2(TYP, LIM) \
  extern Score getScore##TYP(TYP *); \
  extern void onMove##TYP(TYP *, TYP##Index to); \
  extern void onNew##TYP(TYP##Index i, uint32_t hint); \
  extern void onNewLow##TYP(Score); \
  void onMove_##TYP(void * p, Index to)     { onMove##TYP((TYP*)p, (TYP##Index) {to}); } \
  void onNew_##TYP (Index i, uint32_t hint) { onNew##TYP ((TYP##Index){i}, hint);} \
  Score getScore_##TYP(void * p) { return getScore##TYP((TYP*)p); } \
  TYP tmp##TYP; \
  MeapCallbacks MC##TYP = { &tmp##TYP, getScore_##TYP, onNew_##TYP, onMove_##TYP, onNewLow##TYP } ; \
  void meapInsert##TYP(TYP ** pNew, uint32_t hint) { meapInsert(headOf##TYP##s, &MC##TYP, (void**)pNew, hint); } \
  void meapRemove##TYP(TYP##Index i) { meapRemove(headOf##TYP##s, &MC##TYP, i.i); } \
  void meapReview##TYP(TYP##Index i) { meapReview(headOf##TYP##s, &MC##TYP, i.i); } \



