

typedef uint64_t Score;

typedef void (*OnMove) (void *, Index);
typedef Score (*OnScore) (void *);
typedef void (*OnNewLow) (Score low);
typedef struct { void * tmp; OnScore onScore; OnMove onMove; OnNewLow onNewLow; } MeapCallbacks;
void meapInsert(Pilehead * ph, MeapCallbacks * mc, void * proto);
void meapRemove(Pilehead * ph, MeapCallbacks * mc, Index iCur);

#define MAKEMEAP1(TYP) \
  MAKEPILE1(TYP) \

#define MAKEMEAP2(TYP, LIM) \
  MAKEPILE2(TYP, LIM) \
  extern Score score##TYP(TYP *); \
  extern void onMove##TYP(TYP *, TYP##Index to); \
  extern void onNewLow##TYP(Score); \
  void onMove_##TYP(void * p, Index to) { onMove##TYP((TYP*)p, (TYP##Index) {to}); } \
  Score score_##TYP(void * p) { return score##TYP((TYP*)p); } \
  TYP tmp##TYP; \
  MeapCallbacks MC##TYP = { &tmp##TYP, score_##TYP, onMove_##TYP, onNewLow##TYP } ; \
  void meapInsert##TYP(TYP proto) { meapInsert(headOf##TYP##s, &MC##TYP, (void*)&proto); } \
  void meapRemove##TYP(TYP##Index i) { meapRemove(headOf##TYP##s, &MC##TYP, i.i); } \





