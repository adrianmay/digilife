#include "pile.h"

typedef void (*OnMove) (void *, Index);
typedef Score (*OnScore) (void *);
typedef void (*OnNewLow) (Score low);
typedef void (*OnNew) (Index iMeap, uint32_t hint);
typedef struct { void * tmp; OnScore getScore; OnNew onNew; OnMove onMove; OnNewLow onNewLow; } MeapCallbacks;
void meapInsert(Pilehead * ph, MeapCallbacks * mc, void ** pNew, uint32_t hint);
void meapRemove(Pilehead * ph, MeapCallbacks * mc, Index iCur);
void meapReview(Pilehead * ph, MeapCallbacks * mc, Index iCur);

#define MAKEMEAP1(TYP) \
  MAKEPILE1(TYP) \

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



