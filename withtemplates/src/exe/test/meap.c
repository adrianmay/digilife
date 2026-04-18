/*
#include <string.h>
#include "test.h"
#include "meap.h"

MAKEMEAP1(MyMeap)
typedef struct { Tocks tocks; } MyMeap;
MyMeap prototypeMyMeap = { 0 };
MAKEMEAP2(MyMeap, GIGA)


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
MyMeapIndex iMeap;

#define CHOMPNOTHING 1
uint doWhat;
void expect(uint what) {doWhat |= what;}
int fullChompN; Score fullChompP[10];
void expectFullChomp(int n, Score * p) { fullChompN=n; memcpy(fullChompP, p, n*sizeof(Score)); }

Score getScoreMyMeap(MyMeap * p) {return p->tocks/0x10; }
void onNewMyMeap(MyMeapIndex i, uint32_t hint) { getMyMeap(i)->tocks = hint; }  
void onMoveMyMeap(MyMeap * p, MyMeapIndex i) {} //{ printf("Moving 0x%x to %d\n", p->tocks, i.i); fullChompP[i.i]=p->tocks; }  
bool meapAppealMyMeap(MyMeap * p) {return false; }

bool setupEmpty() { openMyMeapPile(); return true; }

bool setupSingleton() { 
  setupEmpty(); 
  meapInsertMyMeap(&iMeap, &pMeap, 0x88);
  expect(CHOMPNOTHING);
  expectFullChomp(1, (Score []){0x88});
  return true;
}

bool setup2Inc() { 
  setupSingleton(); 
  meapInsertMyMeap(&iMeap, &pMeap, 0xc8);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0xc8});
  return true;
}
bool setup2Dec() { 
  setupSingleton(); 
  meapInsertMyMeap(&iMeap, &pMeap, 0x48);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x48, 0x88});
  return true;
}
bool setup2CloseInc() { 
  setupSingleton(); 
  meapInsertMyMeap(&iMeap, &pMeap, 0x89);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0x89});
  return true;
}
bool setup2CloseDec() { 
  setupSingleton(); 
  meapInsertMyMeap(&iMeap, &pMeap, 0x87);
  expectFullChomp(2, (Score []){0x88, 0x87}); // Cos score (/16) is same
  expect(CHOMPNOTHING);
  return true;
}

bool setup3(Score a, Score b, Score c) {
  setupEmpty(); 
  meapInsertMyMeap(&iMeap, &pMeap, a);
  meapInsertMyMeap(&iMeap, &pMeap, b);
  meapInsertMyMeap(&iMeap, &pMeap, c);
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

int counts[numMeapSetups ] = {0,1,2,2,2,2,3,3,3,3,3,3,3};
bool count(int killed) {
  Pilehead * ph = headOfMyMeaps;
  Index cnt = getUsr(ph);
  Index exp = counts[setupNum]-killed;
  assertIntM(cnt, exp);
  return true;
}

bool testMeap1() {
  return
    count(0) &&
    ordered() && 
    ordered() && 
    true;
}

bool testMeap2() {
  return
    count(0) &&
    true;
}

bool testMeap3() {
  return
    (meapRemoveMyMeap((MyMeapIndex){2}),true) &&
    count(setupNum>0 ? 1 : 0) &&
    ordered() &&
    true;
}


bool testMeap4() {
  Pilehead * ph = headOfMyMeaps;
  Index cnt = getUsr(ph);
  if (cnt==0) return true;
  for (int a=0;a<500;a++) {
    MyMeapIndex i = (MyMeapIndex) {a%cnt};
    MyMeap * p = getMyMeap(i);
    p->tocks = rand()%0x100;
    meapReviewMyMeap(i);
    ordered();
  }
  return true;
}

BV testers[] = {testMeap1, testMeap2, testMeap3, testMeap4};
#define numMeapTesters (sizeof(testers)/sizeof(BV))

void cleanupMeap() { closeMyMeapPile(1); }

bool meap() { 
  for (testNum=0;testNum<numMeapTesters;testNum++) {
    for (setupNum=0;setupNum<numMeapSetups; setupNum++) {
      doWhat=0; fullChompN=0;
      bkt(setterUppers[setupNum], testers[testNum], cleanupMeap);
    }
  }
  return true;
}
*/
