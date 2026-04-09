#include <string.h>

MAKEMEAP1(MyMeap)
typedef struct { Tocks tocks; } MyMeap;
MyMeap prototypeMyMeap = { 0 };
MAKEMEAP2(MyMeap, GIGA)

Score getScoreMyMeap(MyMeap * p) {return p->tocks/0x10; }
void onNewMyMeap(MyMeapIndex i, uint32_t hint) { getMyMeap(i)->tocks = hint; }  
void onMoveMyMeap(MyMeap *, MyMeapIndex) {}  
void onNewLowMyMeap(Score s) {}

#define assertWholeMeap(pExp, N) { \
  printf("Meap setup %d: ", setupNum); \
  Index tot = getUsr(headOfMyMeaps); \
  assertInt(tot, N); \
  for (uint32_t a = 0; a<N; a++) { \
    MyMeap * v = getMyMeap((MyMeapIndex) {a}); \
    assertIntHex(v->tocks,pExp[a]); \
  } \
}

#define assertIntM(A, B) {  \
  char s[30]; \
  sprintf(s, "with meap setup %d", setupNum); \
  assertIntSufHex(A, B, s); \
}

MyMeap * pMeap;

#define CHOMPNOTHING 1
uint doWhat;
void expect(uint what) {doWhat |= what;}
int fullChompN; Score fullChompP[10];
void expectFullChomp(int n, Score * p) { fullChompN=n; memcpy(fullChompP, p, n*sizeof(Score)); }

bool setupEmpty() { openMyMeapPile(); return true; }

bool setupSingleton() { 
  setupEmpty(); 
  meapInsertMyMeap(&pMeap, 0x88);
  expect(CHOMPNOTHING);
  expectFullChomp(1, (Score []){0x88});
  return true;
}

bool setup2Inc() { 
  setupSingleton(); 
  meapInsertMyMeap(&pMeap, 0xc8);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0xc8});
  return true;
}
bool setup2Dec() { 
  setupSingleton(); 
  meapInsertMyMeap(&pMeap, 0x48);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x48, 0x88});
  return true;
}
bool setup2CloseInc() { 
  setupSingleton(); 
  meapInsertMyMeap(&pMeap, 0x89);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0x89});
  return true;
}
bool setup2CloseDec() { 
  setupSingleton(); 
  meapInsertMyMeap(&pMeap, 0x87);
  expectFullChomp(2, (Score []){0x88, 0x87}); // Cos score (/16) is same
  expect(CHOMPNOTHING);
  return true;
}

bool setup3(Score a, Score b, Score c) {
  setupEmpty(); 
  meapInsertMyMeap(&pMeap, a);
  meapInsertMyMeap(&pMeap, b);
  meapInsertMyMeap(&pMeap, c);
  expectFullChomp(3, (Score []){0x18, 0x28, 0x38});
  return true;
}

bool setup123()   { return setup3(0x18,0x28,0x38) || true; }
bool setup132()   { return setup3(0x18,0x38,0x28) || true; }
bool setup213()   { return setup3(0x28,0x18,0x38) || true; }
bool setup231()   { return setup3(0x28,0x38,0x18) || true; }
bool setup312()   { return setup3(0x38,0x18,0x28) || true; }
bool setup321()   { return setup3(0x38,0x28,0x18) || true; }
bool setup3Same() { 
  setup3(0x37,0x38,0x39); 
  expect(CHOMPNOTHING);
  expectFullChomp(3, (Score []){0x37,0x39,0x38}); // Shouldn't be fussy about the order
  return true;
}

typedef bool (*BV)();

BV setterUppers[] = {
  setupEmpty, setupSingleton, 
  setup2Inc, setup2Dec, setup2CloseInc, setup2CloseDec,
  setup123, setup132, setup213, setup231, setup312, setup321, setup3Same
};

#define numMeapSetups (sizeof(setterUppers)/sizeof(BV))

int setupNum, testNum;

bool ordered() {
  Pilehead * ph = headOfMyMeaps;
  Index cnt = getUsr(ph);
  for (Index iCur=1;iCur<cnt;iCur++) {
    Score sCur = MCMyMeap.getScore(findInPile(ph, iCur));
    Index iPar = parent(iCur);
    Score sPar = MCMyMeap.getScore(findInPile(ph, iPar));
    bool ok = sPar <= sCur;
    assertIntM(ok, true);
  }
  return true;
}

int counts[numMeapSetups] = {0,1,2,2,2,2,3,3,3,3,3,3,3};
bool count() {
  Pilehead * ph = headOfMyMeaps;
  Index cnt = getUsr(ph);
  Index exp = counts[setupNum];
  assertIntM(cnt, exp);
  return true;
}

bool chompOk() {
  MyMeap m;
  int res = chompMyMeap(0x2, &m);
  int exp = (setupNum==0)          ? -1 :
            (doWhat&CHOMPNOTHING) ?  0 : 
            1;
  assertIntM(res, exp);
  return true;
}

bool chompMore() {
  MyMeap m;
  for (int i=0;i<fullChompN;i++) {
    int res = chompMyMeap(100000, &m);
    int exp = fullChompP[i];
    //printf("chompMore: setup=%d, i=%d, res=%d, tocks=0x%x, exp=0x%x\n", setupNum, i, res, m.tocks, exp);
    assertIntM(m.tocks, exp);
    assertIntM(res, 1);
  }
  int res = chompMyMeap(100000, &m);
  assertIntM(res, -1);
  return true;
}

bool testMeap1() {
  return
    count() &&
    ordered() && 
    chompOk() &&
    ordered() && 
    true;
}

bool testMeap2() {
  return
    count() &&
    chompMore() &&
    true;
}

bool testMeap3() {
  return
    //(meapRemoveMyMeap((MyMeapIndex){2}),true) &&
    //chompMoreWithout2() &&
    true;
}

BV testers[] = {testMeap1, testMeap2, testMeap3};
#define numMeapTesters (sizeof(testers)/sizeof(BV))

void cleanupMeap() { closeMyMeapPile(2); }

bool meap() { 
  for (testNum=0;testNum<numMeapTesters;testNum++) {
    for (setupNum=0;setupNum<numMeapSetups; setupNum++) {
      doWhat=0; fullChompN=0;
      bkt(setterUppers[setupNum], testers[testNum], cleanupMeap);
    }
  }
  return true;
}



// bool testMeapOld() {
//   meapInsertMyMeap(&pMeap, 0x30);
//   meapInsertMyMeap(&pMeap, 0x20);
//   Index exp3[] = {0x20, 0x30};
//   assertWholeMeap(exp3, 2);
//   meapInsertMyMeap(&pMeap, 0x50);
//   meapInsertMyMeap(&pMeap, 0x10);
//   meapInsertMyMeap(&pMeap, 0x08);
//   meapInsertMyMeap(&pMeap, 0x18);
//   meapRemoveMyMeap((MyMeapIndex){0});
//   Index exp1[] = {0x10, 0x20, 0x18, 0x30, 0x50};
//   assertWholeMeap(exp1, 5);
//   meapRemoveMyMeap((MyMeapIndex){0});
//   Index exp2[] = {0x18, 0x20, 0x50, 0x30};
//   assertWholeMeap(exp2, 4);
//   // Need a lot more edge cases
//   return true;
// }

