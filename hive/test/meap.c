MAKEMEAP1(MyMeap)
typedef struct { Tocks tocks; } MyMeap;
MyMeap prototypeMyMeap = { 0 };
MAKEMEAP2(MyMeap, GIGA)

Score getScoreMyMeap(MyMeap * p) {return p->tocks; }
void onNewMyMeap(MyMeapIndex i, uint32_t hint) { getMyMeap(i)->tocks = hint; }  
void onMoveMyMeap(MyMeap *, MyMeapIndex) {}  
void onNewLowMyMeap(Score s) {}

bool assertWholeMeap(Index * pExp, int n) {
  Index tot = getUsr(headOfMyMeaps);
  assertInt(tot, n);
  for (uint32_t a = 0; a<n; a++) {
    MyMeap * v = getMyMeap((MyMeapIndex) {a});
    assertInt(v->tocks,pExp[a]);
  }
  return true;
}

bool testMeap() {
  MyMeap * pMeap;
  openMyMeapPile();
  meapInsertMyMeap(&pMeap, 0x30);
  meapInsertMyMeap(&pMeap, 0x20);
  Index exp3[] = {0x20, 0x30};
  assertWholeMeap(exp3, 2);
  meapInsertMyMeap(&pMeap, 0x50);
  meapInsertMyMeap(&pMeap, 0x10);
  meapInsertMyMeap(&pMeap, 0x08);
  meapInsertMyMeap(&pMeap, 0x18);
  meapRemoveMyMeap((MyMeapIndex){0});
  Index exp1[] = {0x10, 0x20, 0x18, 0x30, 0x50};
  assertWholeMeap(exp1, 5);
  meapRemoveMyMeap((MyMeapIndex){0});
  Index exp2[] = {0x18, 0x20, 0x50, 0x30};
  assertWholeMeap(exp2, 4);
  // Need a lot more edge cases
  return true;
}

void cleanupMeap() { closeMyMeapPile(false); hideMyMeapPile(); }
bool meap() { return bkt(nowt,testMeap,cleanupMeap); }
