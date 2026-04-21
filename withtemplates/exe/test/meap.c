#include <string.h>
#include <stdlib.h>
#include "Junk_meap/h.h"
#include "test.h"

Score getJunkScore(Junk * pJ) { return pJ->tocks; }
void  onNewJunk(JunkIndex iJ, Index hint) { }
void  onMoveJunk(Junk * pJ, JunkIndex to) { }

#define assertWholeMeap(pExp, N) { \
  printf("Meap setup %d: ", setupNum); \
  Index tot = pileOfJunks.getUsr(); \
  assertInt(tot, N); \
  for (uint32_t a = 0; a<N; a++) { \
    Junk * v = getJunk((Junk) {a}); \
    assertIntHex(v->tocks,pExp[a]); \
  } \
}

#define assertIntM(A, B) {  \
  char s[30]; \
  sprintf(s, "with meap setup %d", setupNum); \
  assertIntSufHex(A, B, s); \
}

Junk * pJunk;
JunkIndex iJunk;

#define CHOMPNOTHING 1
int doWhat;
void expect(int what) {doWhat |= what;}
int fullChompN; Score fullChompP[10];
void expectFullChomp(int n, Score * p) { fullChompN=n; memcpy(fullChompP, p, n*sizeof(Score)); }

bool setupEmpty() { pileOfJunks.open(); return true; }

bool setupSingleton() { 
  setupEmpty(); 
  meapOfJunks.insert(&iJunk, &pJunk, 0x88);
  expect(CHOMPNOTHING);
  expectFullChomp(1, (Score []){0x88});
  return true;
}

bool setup2Inc() { 
  setupSingleton(); 
  meapOfJunks.insert(&iJunk, &pJunk, 0xc8);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0xc8});
  return true;
}
bool setup2Dec() { 
  setupSingleton(); 
  meapOfJunks.insert(&iJunk, &pJunk, 0x48);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x48, 0x88});
  return true;
}
bool setup2CloseInc() { 
  setupSingleton(); 
  meapOfJunks.insert(&iJunk, &pJunk, 0x89);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0x89});
  return true;
}
bool setup2CloseDec() { 
  setupSingleton(); 
  meapOfJunks.insert(&iJunk, &pJunk, 0x87);
  expectFullChomp(2, (Score []){0x88, 0x87}); // Cos score (/16) is same
  expect(CHOMPNOTHING);
  return true;
}

bool setup3(Score a, Score b, Score c) {
  setupEmpty(); 
  meapOfJunks.insert(&iJunk, &pJunk, a);
  meapOfJunks.insert(&iJunk, &pJunk, b);
  meapOfJunks.insert(&iJunk, &pJunk, c);
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

int counts[numMeapSetups ] = {0,1,2,2,2,2,3,3,3,3,3,3,3};
bool count(int killed) {
  Index cnt = pileOfJunks.getUsr();
  Index exp = counts[setupNum]-killed;
  assertIntM(cnt, exp);
  return true;
}

bool ordered() {
  bool b = meapOfJunks.checkOrdered();
  assertInt(b, true);
  return b;
}

bool testMeap1() {
  return
    count(0) &&
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
    (meapOfJunks.remove((JunkIndex){2})) &&
    count(setupNum>0 ? 1 : 0) &&
    ordered() &&
    true;
}


bool testMeap4() {
  Index cnt = pileOfJunks.getUsr();
  if (cnt==0) return true;
  for (int a=0;a<500;a++) {
    JunkIndex i = (JunkIndex) {a%cnt};
    Junk * p = pileOfJunks.get(i);
    p->tocks = rand()%0x100;
    meapOfJunks.review(i);
    ordered();
  }
  return true;
}

BV testers[] = {testMeap1};//, testMeap2, testMeap3, testMeap4};
#define numMeapTesters (sizeof(testers)/sizeof(BV))

void cleanupMeap() { pileOfJunks.close(DELETE); }

bool meap() { 
  for (testNum=0;testNum<numMeapTesters;testNum++) {
    for (setupNum=0;setupNum<numMeapSetups; setupNum++) {
      doWhat=0; fullChompN=0;
      bkt(setterUppers[setupNum], testers[testNum], cleanupMeap);
    }
  }
  return true;
}
